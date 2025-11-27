import warnings
warnings.filterwarnings("ignore", category=UserWarning, module='google.protobuf')

import cv2
import time
import torch
import numpy as np
import threading
import joblib
import mediapipe as mp
from collections import deque
import asyncio

from .esp32_client import enviar_comando
from .ia_frases import generar_frase
from .ia_audio import reproducir_audio
from .websocket_state import (
    update_gesto,
    update_estado,
    update_frase,
    update_audio,
    enviar_estado
)
from .config import get_camara_gestos_url

# ==========================================================
# RUTAS MODELO FINAL PHARA NET
# ==========================================================

BASE_MODEL = r"A:\GeneracionImagenes\TrabajoFinal\appMobileSynkrohogar\IAmultiModal\modelos\finalPharaNET"

MODEL_PATH = BASE_MODEL + r"\PharaNET_final.pth"
NORM_PATH = BASE_MODEL + r"\normalizacion.json"
CLASES_PATH = BASE_MODEL + r"\clases.txt"

DEVICE = "cuda" if torch.cuda.is_available() else "cpu"


# ==========================================================
# MODELO GESTURE MLP (finalPharaNET)
# ==========================================================

class GestureMLP(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.fc1 = torch.nn.Linear(126, 512)
        self.fc2 = torch.nn.Linear(512, 256)
        self.fc3 = torch.nn.Linear(256, 128)
        self.fc4 = torch.nn.Linear(128, 64)
        self.out = torch.nn.Linear(64, 11)
        self.relu = torch.nn.ReLU()

    def forward(self, x):
        x = self.relu(self.fc1(x))
        x = self.relu(self.fc2(x))
        x = self.relu(self.fc3(x))
        x = self.relu(self.fc4(x))
        return self.out(x)


# ==========================================================
# CARGA DE MODELO Y NORMALIZACIÓN
# ==========================================================

import json

# Cargar clases
clases = {}
with open(CLASES_PATH, "r") as f:
    for line in f.readlines():
        idx, name = line.strip().split(",")
        clases[int(idx)] = name

# Cargar normalización
with open(NORM_PATH, "r") as f:
    norm = json.load(f)

mean = np.array(norm["mean"])
std = np.array(norm["std"])

# Cargar modelo
model = GestureMLP().to(DEVICE)
model.load_state_dict(torch.load(MODEL_PATH, map_location=DEVICE))
model.eval()

print(f"[✓] Modelo finalPharaNET cargado correctamente")
print(f"[✓] Clases detectables: {list(clases.values())}")


# ==========================================================
# MEDIAPIPE
# ==========================================================

mp_hands = mp.solutions.hands
hands = mp_hands.Hands(max_num_hands=2, min_detection_confidence=0.7)
mp_draw = mp.solutions.drawing_utils


# ==========================================================
# ESTADO IA GESTOS
# ==========================================================

VENTANA = 20
TIEMPO_CONFIANZA = 2.3
TIEMPO_COOLDOWN = 3.0

pred_hist = deque(maxlen=VENTANA)

bloqueado = False
gesto_en_confianza = None
inicio_confianza = None
inicio_cooldown = 0
ultimo_ejecutado = None

# Frame procesado con landmarks para mostrar
frame_procesado = None
frame_lock = threading.Lock()


# ==========================================================
# COMANDOS
# ==========================================================

comandos = {
    "abrirPuerta": "CMD|OPEN_DOOR",
    "modoSuper": "CMD|SUPER_ON",
    "modoNormal": "CMD|NORMAL_ON",
    "modoUltra": "CMD|ULTRA_ON",
    "modoAurora": "CMD|AURORA_ON",
    "auroraSiguiente": "CMD|AURORA_NEXT",
    "auroraAnterior": "CMD|AURORA_PREV",
    "siguienteMelodia": "CMD|CHANGE_MUSIC",
    "anteriorMelodia": "CMD|PREV_MUSIC",
    "reproducirMelodia": "CMD|PLAY_MUSIC",
    "encenderLuces": "CMD|LED_ON",
    "apagarLuces": "CMD|LED_OFF"
}


# ==========================================================
# PUENTE HILO → ASYNC
# ==========================================================

def enviar_estado_async_seguro():
    """
    Evita el error 'coroutine was never awaited'.
    Corre correctamente enviar_estado() desde un HILO.
    """
    try:
        loop = asyncio.get_running_loop()
        loop.create_task(enviar_estado())
    except RuntimeError:
        asyncio.run(enviar_estado())


# ==========================================================
# HILO IA GESTOS
# ==========================================================

def hilo_ia():
    global bloqueado, gesto_en_confianza, inicio_confianza, inicio_cooldown, ultimo_ejecutado, frame_procesado

    # Conectar a cámara IP de gestos
    cam_url = get_camara_gestos_url()
    if not cam_url:
        print("[ERROR] URL de cámara de gestos no configurada")
        return
    
    print(f"[INFO] Conectando a cámara de gestos: {cam_url}")
    cap = cv2.VideoCapture(cam_url)
    
    if not cap.isOpened():
        print("[ERROR] No se pudo conectar a la cámara de gestos")
        return
    
    print("[OK] IA de gestos conectada correctamente")

    while True:

        # --------------------------------------------------
        # LEER FRAME desde cámara IP
        # --------------------------------------------------
        ret, frame = cap.read()
        if not ret:
            time.sleep(0.05)
            continue
        
        # Voltear horizontalmente para efecto espejo
        frame = cv2.flip(frame, 1)

        # Trabajar directamente sobre el frame
        rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        result = hands.process(rgb)

        puntos = []

        # --------------------------------------------------
        # LANDMARKS
        # --------------------------------------------------
        if result.multi_hand_landmarks:
            for h in result.multi_hand_landmarks:
                for lm in h.landmark:
                    puntos.extend([lm.x, lm.y, lm.z])
                
                # DIBUJAR LANDMARKS en el frame (directo, sin copia)
                mp_draw.draw_landmarks(frame, h, mp_hands.HAND_CONNECTIONS)

            if len(puntos) > 126:
                puntos = puntos[:126]
            while len(puntos) < 126:
                puntos.append(0.0)

            # Normalizar con mean y std del modelo
            X = np.array(puntos, dtype=np.float32)
            X = (X - mean) / (std + 1e-7)
            X = torch.tensor(X, dtype=torch.float32).unsqueeze(0).to(DEVICE)

            with torch.no_grad():
                logits = model(X)
                pred_idx = torch.argmax(logits, dim=1).item()
                pred = clases[pred_idx]

            pred_hist.append(pred)

        else:
            pred = None
            pred_hist.clear()

        # --------------------------------------------------
        # SUAVIZACIÓN
        # --------------------------------------------------
        pred_suavizado = max(set(pred_hist), key=pred_hist.count) if pred_hist else None
        ahora = time.time()

        update_gesto(pred_suavizado or "")
        enviar_estado_async_seguro()

        # --------------------------------------------------
        # DIBUJAR INFORMACIÓN EN EL FRAME
        # --------------------------------------------------
        texto = ""
        color = (0, 255, 0)
        
        if gesto_en_confianza:
            restante = TIEMPO_CONFIANZA - (ahora - inicio_confianza)
            texto = f"Detectando {gesto_en_confianza} ({restante:.1f}s)"
        elif bloqueado:
            restante = TIEMPO_COOLDOWN - (ahora - inicio_cooldown)
            texto = f"Ejecutado '{ultimo_ejecutado}' ({restante:.1f}s)"
            color = (0, 140, 255)
        elif pred_suavizado:
            texto = f"Gesto: {pred_suavizado}"

        if texto:
            # Texto más grande: tamaño 1.2 y grosor 3
            cv2.putText(frame, texto, (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1.2, color, 3)

        # Guardar frame procesado (referencia directa, sin copia)
        with frame_lock:
            frame_procesado = frame

        # --------------------------------------------------
        # CONFIANZA
        # --------------------------------------------------
        if not bloqueado and pred_suavizado in comandos:

            if gesto_en_confianza != pred_suavizado:
                gesto_en_confianza = pred_suavizado
                inicio_confianza = ahora

            elif (ahora - inicio_confianza) >= TIEMPO_CONFIANZA:

                cmd = comandos[gesto_en_confianza]
                enviar_comando(cmd)

                frase = generar_frase(gesto_en_confianza)
                reproducir_audio(frase)

                update_estado(cmd)
                update_frase(frase)
                update_audio("on")

                enviar_estado_async_seguro()

                ultimo_ejecutado = gesto_en_confianza
                inicio_cooldown = ahora
                bloqueado = True
                gesto_en_confianza = None
                inicio_confianza = None

        # --------------------------------------------------
        # COOLDOWN
        # --------------------------------------------------
        elif bloqueado:
            if (ahora - inicio_cooldown) >= TIEMPO_COOLDOWN:
                bloqueado = False
                update_audio("off")
                enviar_estado_async_seguro()

        # Reducir CPU con pequeño sleep
        time.sleep(0.02)


# ==========================================================
# INICIAR HILO IA
# ==========================================================

def iniciar_hilo_ia():
    hilo = threading.Thread(target=hilo_ia, daemon=True)
    hilo.start()


# ==========================================================
# OBTENER FRAME PROCESADO (con landmarks y texto)
# ==========================================================

def obtener_frame_procesado():
    """Retorna el frame procesado con landmarks y texto de confianza"""
    with frame_lock:
        if frame_procesado is not None:
            return True, frame_procesado
        return False, None

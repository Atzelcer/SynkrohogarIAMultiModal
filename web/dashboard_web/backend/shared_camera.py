"""
Cámara compartida para evitar conflictos entre IA y streaming.
"""
import cv2
import threading
import time
import numpy as np


class SharedCamera:
    def __init__(self, camera_index=0):
        self.camera_index = camera_index
        self.frame = None
        self.running = False
        self.lock = threading.Lock()
        self.cap = None
        
    def start(self):
        """Inicia la captura de cámara en un hilo separado"""
        if self.running:
            return
            
        print(f"[INFO] Iniciando cámara compartida (índice {self.camera_index})...")
        
        # Intentar con DSHOW primero (Windows)
        self.cap = cv2.VideoCapture(self.camera_index, cv2.CAP_DSHOW)
        
        if not self.cap.isOpened():
            print("[WARN] DSHOW falló, intentando backend por defecto...")
            self.cap = cv2.VideoCapture(self.camera_index)
        
        if not self.cap.isOpened():
            print("[ERROR] No se pudo abrir la cámara")
            return False
        
        # Configurar para máximo rendimiento
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
        self.cap.set(cv2.CAP_PROP_FPS, 30)
        self.cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)
        self.cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*'MJPG'))
        
        self.running = True
        threading.Thread(target=self._capture_loop, daemon=True).start()
        print("[OK] Cámara compartida iniciada correctamente")
        return True
    
    def _capture_loop(self):
        """Loop de captura continua"""
        while self.running:
            ret, frame = self.cap.read()
            if ret:
                with self.lock:
                    self.frame = frame
            time.sleep(0.01)  # ~100 FPS máximo
    
    def read(self):
        """Lee el último frame capturado"""
        with self.lock:
            if self.frame is None:
                # Retornar frame negro si no hay imagen
                return False, np.zeros((480, 640, 3), dtype=np.uint8)
            return True, self.frame.copy()
    
    def stop(self):
        """Detiene la captura"""
        self.running = False
        if self.cap:
            self.cap.release()
        print("[INFO] Cámara compartida detenida")


# Instancia global única
_shared_camera = None

def get_shared_camera(camera_index=0):
    """Obtiene la instancia única de la cámara compartida"""
    global _shared_camera
    if _shared_camera is None:
        _shared_camera = SharedCamera(camera_index)
        _shared_camera.start()
    return _shared_camera

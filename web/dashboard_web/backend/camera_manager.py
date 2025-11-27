import cv2
import time
import numpy as np
from .ia_gestos import obtener_frame_procesado
from .config import get_camara_casa_url


def generar_stream_gestos():
    """
    Stream de video usando frames procesados por IA (con landmarks y texto).
    Muestra exactamente lo que el hilo de IA está detectando desde la cámara IP 1.
    """
    print("[INFO] Iniciando stream de gestos (con landmarks desde cámara IP)...")
    
    frame_count = 0

    while True:
        # Obtener frame procesado del hilo de IA (con landmarks dibujados)
        ok, frame = obtener_frame_procesado()
        
        if not ok or frame is None:
            time.sleep(0.01)
            continue

        frame_count += 1
        
        # Procesar solo cada 2 frames (mejor balance velocidad/calidad)
        if frame_count % 2 != 0:
            continue

        # Mayor resolución para mejor calidad
        frame = cv2.resize(frame, (640, 480), interpolation=cv2.INTER_LINEAR)

        # Mejor calidad de compresión
        ok, jpg = cv2.imencode(".jpg", frame, [
            cv2.IMWRITE_JPEG_QUALITY, 75
        ])
        
        if not ok:
            continue

        yield (
            b"--frame\r\n"
            b"Content-Type: image/jpeg\r\n\r\n" +
            jpg.tobytes() +
            b"\r\n"
        )


def generar_stream_celular():
    """
    Stream desde la segunda cámara IP (vista de la casa).
    """
    url = get_camara_casa_url()
    
    if not url:
        print("[ERROR] URL de cámara de casa no configurada.")
        return

    print(f"[INFO] Intentando conectar a cámara de casa: {url}")
    
    # Configurar timeout y opciones para la conexión
    cap = None
    retry = 0
    max_retries = 5
    
    while retry < max_retries:
        try:
            # Intentar abrir con opciones específicas para streams HTTP
            cap = cv2.VideoCapture(url, cv2.CAP_FFMPEG)
            
            # Configurar buffer bajo para reducir latencia
            cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)
            
            # Dar tiempo para que se conecte
            time.sleep(1)
            
            if cap.isOpened():
                # Intentar leer un frame de prueba
                ret, test_frame = cap.read()
                if ret and test_frame is not None:
                    print(f"[OK] Cámara IP conectada correctamente en intento {retry + 1}")
                    break
                else:
                    print(f"[WARN] Conexión establecida pero no se reciben frames, reintentando...")
                    cap.release()
            else:
                print(f"[WARN] No se pudo abrir la cámara (intento {retry + 1}/{max_retries})")
            
        except Exception as e:
            print(f"[ERROR] Excepción al conectar: {e}")
        
        retry += 1
        if retry < max_retries:
            print(f"[INFO] Reintentando en 2 segundos...")
            time.sleep(2)
    
    if not cap or not cap.isOpened():
        print("[ERROR] No se pudo conectar a la cámara IP después de todos los intentos.")
        print("[TIPS] Verifica que:")
        print("  1. IP Webcam esté ejecutándose en tu Android")
        print("  2. La IP y puerto sean correctos")
        print("  3. Tu PC y celular estén en la misma red WiFi")
        print(f"  4. Puedas abrir {url} en tu navegador")
        return

    print("[INFO] Streaming de cámara IP iniciado.")

    frame_count = 0
    error_count = 0
    max_errors = 30

    while True:
        try:
            ok, frame = cap.read()

            if not ok or frame is None:
                error_count += 1
                if error_count > max_errors:
                    print("[ERROR] Demasiados errores consecutivos, reconectando...")
                    cap.release()
                    time.sleep(2)
                    cap = cv2.VideoCapture(url, cv2.CAP_FFMPEG)
                    error_count = 0
                    continue
                time.sleep(0.1)
                continue

            error_count = 0  # Reset contador de errores
            frame_count += 1

            # Redimensionar para mejor rendimiento manteniendo calidad
            frame = cv2.resize(frame, (640, 480), interpolation=cv2.INTER_LINEAR)

            # Calidad óptima para streaming
            ok, jpg = cv2.imencode(".jpg", frame, [cv2.IMWRITE_JPEG_QUALITY, 80])
            if not ok:
                continue

            yield (
                b"--frame\r\n"
                b"Content-Type: image/jpeg\r\n\r\n" +
                jpg.tobytes() +
                b"\r\n"
            )

            time.sleep(0.025)

        except Exception as e:
            print(f"[ERROR] Error en streaming: {e}")
            time.sleep(0.5)
            continue

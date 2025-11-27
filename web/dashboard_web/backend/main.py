from fastapi import FastAPI
from fastapi.responses import StreamingResponse, JSONResponse
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles

from .config import pedir_urls_camaras, get_camara_gestos_url, get_camara_casa_url
from .camera_manager import generar_stream_gestos, generar_stream_celular
from .ia_gestos import iniciar_hilo_ia
from .websocket_state import router as ws_router


# =====================================================
# INICIO FASTAPI
# =====================================================

app = FastAPI()

# Pedir URLs de AMBAS cámaras + ESP32 IP ANTES DE levantar el servidor
gestos_url, casa_url, esp32_ip = pedir_urls_camaras()
print(f"[✓] Configuración cargada: Gestos={gestos_url}, Casa={casa_url}, ESP32={esp32_ip if esp32_ip else 'No configurado'}")


# =====================================================
# CORS
# =====================================================

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)


# =====================================================
# SERVIR FRONTEND
# =====================================================

app.mount("/frontend", StaticFiles(directory="frontend"), name="frontend")


# =====================================================
# ENDPOINTS
# =====================================================

@app.get("/")
def home():
    return {"status": "SynkroHogar Backend OK"}


@app.get("/url_camara")
def url_camara():
    return JSONResponse({
        "gestos": get_camara_gestos_url(),
        "casa": get_camara_casa_url()
    })


@app.get("/video_gestos")
def video_gestos():
    return StreamingResponse(
        generar_stream_gestos(),
        media_type="multipart/x-mixed-replace; boundary=frame"
    )


@app.get("/video_casa")
def video_casa():
    return StreamingResponse(
        generar_stream_celular(),
        media_type="multipart/x-mixed-replace; boundary=frame"
    )


# WebSocket router
app.include_router(ws_router)


# =====================================================
# INICIO IA EN SEGUNDO PLANO
# =====================================================

iniciar_hilo_ia()
print(">>> IA de gestos iniciada correctamente.")

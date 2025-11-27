from fastapi import WebSocket, APIRouter
import asyncio

router = APIRouter()

# Estado global enviado al dashboard
estado_actual = {
    "gesto": "",
    "estado": "",
    "frase": "",
    "audio": "off"
}

clientes = []

# ==========================================================
# WEBSOCKET
# ==========================================================

async def ws_endpoint(ws: WebSocket):
    await ws.accept()
    clientes.append(ws)

    try:
        while True:
            # No recibimos nada, solo mantenemos viva la conexión
            await ws.receive_text()
    except:
        pass
    finally:
        if ws in clientes:
            clientes.remove(ws)


router.add_api_websocket_route("/ws_estado", ws_endpoint)


# ==========================================================
# ACTUALIZADORES
# ==========================================================

def update_gesto(v):
    estado_actual["gesto"] = v


def update_estado(v):
    estado_actual["estado"] = v


def update_frase(v):
    estado_actual["frase"] = v


def update_audio(v):
    estado_actual["audio"] = v


# ==========================================================
# ENVÍO CORRECTO DE ESTADO ASYNC
# ==========================================================

async def enviar_estado():
    """
    Envía el estado actual a todos los clientes WebSocket conectados.
    """
    muertos = []

    for ws in clientes:
        try:
            await ws.send_json(estado_actual)
        except:
            muertos.append(ws)

    # limpiar desconectados
    for ws in muertos:
        if ws in clientes:
            clientes.remove(ws)


# ==========================================================
# ENVÍO DESDE HILOS (PUENTE)
# ==========================================================

def enviar_estado_async_seguro():
    """
    Permite llamar enviar_estado() desde un hilo SIN causar errores async.
    """
    try:
        loop = asyncio.get_running_loop()
        loop.create_task(enviar_estado())
    except RuntimeError:
        # Si no hay loop, crear uno
        asyncio.run(enviar_estado())

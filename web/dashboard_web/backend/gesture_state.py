import asyncio
import time

class GestureState:
    def __init__(self):
        self.gesto = None
        self.frase = None
        self.estado = None
        self.cooldown = 0
        self.observers = set()

    async def notify(self):
        data = {
            "gesto": self.gesto,
            "frase": self.frase,
            "estado": self.estado,
            "cooldown": self.cooldown
        }
        for ws in list(self.observers):
            try:
                await ws.send_json(data)
            except:
                self.observers.remove(ws)

    async def set(self, gesto=None, frase=None, estado=None, cooldown=None):
        if gesto is not None:
            self.gesto = gesto
        if frase is not None:
            self.frase = frase
        if estado is not None:
            self.estado = estado
        if cooldown is not None:
            self.cooldown = cooldown
        await self.notify()

    def fake_loop(self):
        while True:
            asyncio.run(self.set(
                gesto="modoNormal",
                frase="Modo normal activado",
                estado="Reconociendo",
                cooldown=2
            ))
            time.sleep(3)

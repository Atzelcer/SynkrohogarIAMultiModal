import numpy as np
import threading
import sounddevice as sd
from TTS.api import TTS

tts = TTS("tts_models/es/css10/vits")

def reproducir_audio(texto):
    def tarea():
        try:
            wav = tts.tts(texto)
            wav = np.array(wav, dtype=np.float32)
            sd.play(wav, 22050)
            sd.wait()
        except:
            pass
    threading.Thread(target=tarea, daemon=True).start()

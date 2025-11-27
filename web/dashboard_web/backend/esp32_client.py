import threading
import requests
from .config import get_esp32_ip

def enviar_comando(cmd):
    """Envía un comando al ESP32"""
    esp32_ip = get_esp32_ip()
    
    if not esp32_ip:
        print(f"[!] ESP32 no configurado. Comando '{cmd}' no enviado.")
        return
    
    comando_url = f"http://{esp32_ip}:8080/cmd?msg={cmd}"
    
    def tarea():
        try:
            response = requests.get(comando_url, timeout=2)
            if response.status_code == 200:
                print(f"[✓] Comando enviado al ESP32: {cmd}")
            else:
                print(f"[!] ESP32 respondió con código: {response.status_code}")
        except requests.exceptions.Timeout:
            print(f"[✗] Timeout al enviar comando al ESP32: {cmd}")
        except requests.exceptions.ConnectionError:
            print(f"[✗] No se pudo conectar al ESP32 ({esp32_ip})")
        except Exception as e:
            print(f"[✗] Error al enviar comando: {e}")
    
    threading.Thread(target=tarea, daemon=True).start()

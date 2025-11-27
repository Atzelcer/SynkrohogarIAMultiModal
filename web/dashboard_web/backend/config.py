import os
import json

CONFIG_FILE = os.path.join(os.path.dirname(__file__), "camera_config.json")

_CAMARA_GESTOS_URL = None  # Cámara 1: detección de gestos con landmarks
_CAMARA_CASA_URL = None    # Cámara 2: vista de la casa
_ESP32_IP = None           # IP del ESP32


def cargar_config():
    """Carga la configuración desde el archivo JSON"""
    global _CAMARA_GESTOS_URL, _CAMARA_CASA_URL, _ESP32_IP
    
    if os.path.exists(CONFIG_FILE):
        try:
            with open(CONFIG_FILE, 'r') as f:
                config = json.load(f)
                _CAMARA_GESTOS_URL = config.get("gestos")
                _CAMARA_CASA_URL = config.get("casa")
                _ESP32_IP = config.get("esp32_ip")
                print(f"[✓] Configuración cargada desde {CONFIG_FILE}")
                print(f"    Gestos: {_CAMARA_GESTOS_URL}")
                print(f"    Casa: {_CAMARA_CASA_URL}")
                print(f"    ESP32 IP: {_ESP32_IP}")
                return True
        except Exception as e:
            print(f"[!] Error al cargar config: {e}")
    return False


def guardar_config():
    """Guarda la configuración en el archivo JSON"""
    config = {
        "gestos": _CAMARA_GESTOS_URL,
        "casa": _CAMARA_CASA_URL,
        "esp32_ip": _ESP32_IP
    }
    with open(CONFIG_FILE, 'w') as f:
        json.dump(config, f, indent=2)
    print(f"[✓] Configuración guardada en {CONFIG_FILE}")


def get_camara_gestos_url():
    """Retorna la URL de la cámara de gestos (celular 1)"""
    return _CAMARA_GESTOS_URL


def get_camara_casa_url():
    """Retorna la URL de la cámara de la casa (celular 2)"""
    return _CAMARA_CASA_URL


def get_esp32_ip():
    """Retorna la IP del ESP32"""
    return _ESP32_IP


def verificar_esp32(ip):
    """Verifica si el ESP32 responde en la IP dada"""
    import requests
    try:
        response = requests.get(f"http://{ip}:8080/cmd?msg=CMD|PING", timeout=2)
        return response.status_code == 200
    except:
        return False


def pedir_urls_camaras():
    global _CAMARA_GESTOS_URL, _CAMARA_CASA_URL, _ESP32_IP
    
    # Intentar cargar desde archivo
    if cargar_config():
        respuesta = input("\n¿Quieres usar esta configuración? (s/n): ").strip().lower()
        if respuesta == 's':
            # Verificar ESP32
            if _ESP32_IP:
                print(f"\n[INFO] Verificando conexión con ESP32 ({_ESP32_IP})...")
                if verificar_esp32(_ESP32_IP):
                    print("[✓] ESP32 conectado correctamente")
                else:
                    print("[!] ESP32 no responde, necesitas configurarlo nuevamente")
                    _ESP32_IP = None
            
            if _CAMARA_GESTOS_URL and _CAMARA_CASA_URL and _ESP32_IP:
                return (_CAMARA_GESTOS_URL, _CAMARA_CASA_URL, _ESP32_IP)

    print("\n" + "="*60)
    print("  CONFIGURACIÓN COMPLETA - SYNKROHOGAR IA MULTIMODAL")
    print("="*60)
    print("\nVamos a configurar:")
    print("  1. Dos cámaras IP Webcam (Android)")
    print("  2. ESP32 para control domótico")
    print("\n" + "="*60 + "\n")
    
    # Cámara 1: Gestos (con landmarks y detección)
    print("📱 CÁMARA 1 - Detección de Gestos")
    print("Esta cámara mostrará los landmarks de las manos y la confianza")
    print("Ejemplo: 192.168.0.22:8080\n")
    ip_gestos = input("Introduce IP:Puerto del celular 1 (gestos): ").strip()
    
    if not ip_gestos:
        print("ERROR: No escribiste nada. Intenta de nuevo.")
        return pedir_urls_camaras()
    
    _CAMARA_GESTOS_URL = f"http://{ip_gestos}/video"
    print(f"[✓] Cámara gestos: {_CAMARA_GESTOS_URL}\n")
    
    # Cámara 2: Casa
    print("🏠 CÁMARA 2 - Vista de la Casa")
    print("Esta cámara mostrará la vista general de la casa")
    print("Ejemplo: 192.168.0.23:8080\n")
    ip_casa = input("Introduce IP:Puerto del celular 2 (casa): ").strip()
    
    if not ip_casa:
        print("ERROR: No escribiste nada. Intenta de nuevo.")
        return pedir_urls_camaras()
    
    _CAMARA_CASA_URL = f"http://{ip_casa}/video"
    print(f"[✓] Cámara casa: {_CAMARA_CASA_URL}\n")
    
    # ESP32
    print("🔌 ESP32 - Control Domótico")
    print("Introduce la IP del ESP32 (sin puerto)")
    print("Ejemplo: 192.168.0.100\n")
    
    while True:
        esp32_ip = input("Introduce IP del ESP32: ").strip()
        
        if not esp32_ip:
            print("[!] No escribiste nada. Intenta de nuevo.")
            continue
        
        print(f"\n[INFO] Verificando conexión con ESP32 ({esp32_ip})...")
        
        if verificar_esp32(esp32_ip):
            _ESP32_IP = esp32_ip
            print(f"[✓] ESP32 conectado correctamente en {esp32_ip}\n")
            break
        else:
            print(f"[✗] ESP32 NO responde en {esp32_ip}")
            print("\n[TIPS] Verifica que:")
            print("  1. El ESP32 esté encendido y conectado a WiFi")
            print("  2. La IP sea correcta")
            print("  3. El ESP32 y tu PC estén en la misma red")
            print("  4. El servidor HTTP del ESP32 esté en el puerto 8080\n")
            
            respuesta = input("¿Quieres reintentar con otra IP? (s/n): ").strip().lower()
            if respuesta != 's':
                print("[!] Configurando sin ESP32 (los comandos no se enviarán)")
                _ESP32_IP = None
                break
    
    print("\n" + "="*60)
    print("✅ CONFIGURACIÓN COMPLETA")
    print("="*60)
    print(f"  Cámara Gestos: {_CAMARA_GESTOS_URL}")
    print(f"  Cámara Casa:   {_CAMARA_CASA_URL}")
    print(f"  ESP32:         {_ESP32_IP if _ESP32_IP else 'NO CONFIGURADO'}")
    print("="*60 + "\n")
    
    # Guardar configuración
    guardar_config()
    
    return (_CAMARA_GESTOS_URL, _CAMARA_CASA_URL, _ESP32_IP)

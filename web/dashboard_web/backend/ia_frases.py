import sys
import os

# ============================================================
# UBICACIÓN DEL MODELO PHRASE NET OMEGA
# ============================================================

PHRASE_BASE = r"A:\GeneracionImagenes\TrabajoFinal\appMobileSynkrohogar\IAmultiModal\modelos\PhraseNetOmega"

# Agregar al path
if PHRASE_BASE not in sys.path:
    sys.path.append(PHRASE_BASE)

# Importar módulo de inferencia
from PhraseNetOmega3_infer import cargar_modelo, generar_cmd


# ============================================================
# CARGAR MODELO UNA SOLA VEZ
# ============================================================

sp, modelo = cargar_modelo(PHRASE_BASE)


# ============================================================
# FUNCIÓN PARA GENERAR FRASES
# ============================================================

def generar_frase(cmd):
    """
    Devuelve la frase generada por PhraseNet para el comando detectado.
    """
    return generar_cmd(sp, modelo, cmd)

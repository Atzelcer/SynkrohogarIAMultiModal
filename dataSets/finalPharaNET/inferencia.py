
import torch
import json
import numpy as np

# ==== CARGAR ARCHIVOS ====

BASE = r"A:\GeneracionImagenes\TrabajoFinal\appMobileSynkrohogar\IAmultiModal\modelos\finalPharaNET"

# Cargar clases
clases = {}
with open(BASE + r"\clases.txt") as f:
    for line in f.readlines():
        idx, name = line.strip().split(",")
        clases[int(idx)] = name

# Cargar normalización
with open(BASE + r"\normalizacion.json") as f:
    norm = json.load(f)

mean = np.array(norm["mean"])
std  = np.array(norm["std"])

# Modelo
import torch.nn as nn
class GestureMLP(nn.Module):
    def __init__(self):
        super().__init__()
        self.fc1 = nn.Linear(126, 512)
        self.fc2 = nn.Linear(512, 256)
        self.fc3 = nn.Linear(256, 128)
        self.fc4 = nn.Linear(128, 64)
        self.out = nn.Linear(64, 11)
        self.relu = nn.ReLU()

    def forward(self, x):
        x = self.relu(self.fc1(x))
        x = self.relu(self.fc2(x))
        x = self.relu(self.fc3(x))
        x = self.relu(self.fc4(x))
        return self.out(x)

model = GestureMLP()
model.load_state_dict(torch.load(BASE + r"\PharaNET_final.pth"))
model.eval()

# ==== FUNCIÓN DE INFERENCIA ====

def predecir(vector_126):
    x = np.array(vector_126, dtype=np.float32)
    x = (x - mean) / (std + 1e-7)
    x = torch.tensor(x).unsqueeze(0)
    logits = model(x)
    pred = torch.argmax(logits, dim=1).item()
    return clases[pred]

print("Inferencia lista. Use: predecir(vector)")

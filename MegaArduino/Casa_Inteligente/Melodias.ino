#include <Arduino.h>

#define BUZZER_PIN 8

struct Nota {
  int frecuencia;
  int duracion;
};

const Nota melodia1[] = {{262,300},{294,300},{330,300},{349,300},{392,300}};
const Nota melodia2[] = {{392,200},{370,200},{392,200},{330,400},{392,300}};
const Nota melodia3[] = {{440,250},{440,250},{440,250},{349,250},{392,250}};
const Nota melodia4[] = {{294,200},{330,200},{349,200},{392,200},{440,200}};
const Nota melodia5[] = {{330,300},{330,150},{392,150},{330,150},{262,400}};
const Nota melodia6[] = {{440,250},{494,250},{523,250},{587,250},{659,250}};
const Nota melodia7[] = {{523,200},{494,200},{440,200},{392,200},{349,300}};
const Nota melodia8[] = {{330,300},{370,300},{392,300},{440,300},{494,300}};
const Nota melodia9[] = {{262,250},{294,250},{330,250},{262,250},{330,400}};
const Nota melodia10[] = {{392,150},{392,150},{440,300},{494,300},{523,300}};
const Nota melodia11[] = {{330,150},{392,150},{440,300},{330,300},{294,300}};
const Nota melodia12[] = {{392,150},{440,150},{494,150},{523,200},{440,400}};
const Nota melodia13[] = {{440,150},{392,150},{330,300},{262,300},{294,300}};
const Nota melodia14[] = {{330,200},{392,200},{330,200},{262,400},{262,300}};
const Nota melodia15[] = {{262,250},{330,250},{392,250},{523,250},{440,300}};
const Nota melodia16[] = {{440,150},{392,150},{349,200},{330,200},{294,300}};
const Nota melodia17[] = {{262,250},{330,250},{392,250},{262,250},{330,250}};
const Nota melodia18[] = {{523,200},{494,200},{440,200},{392,200},{330,300}};
const Nota melodia19[] = {{392,150},{392,150},{440,300},{392,300},{330,300}};
const Nota melodia20[] = {{330,200},{294,200},{262,400},{330,300},{440,300}};

const Nota* melodias[] = {
  melodia1, melodia2, melodia3, melodia4, melodia5,
  melodia6, melodia7, melodia8, melodia9, melodia10,
  melodia11, melodia12, melodia13, melodia14, melodia15,
  melodia16, melodia17, melodia18, melodia19, melodia20
};

const int tamanos[] = {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};

int melodiaActual = 0;

void Melodias_begin() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.println("Módulo de melodías iniciado.");
}

void reproducirMelodia(int index) {
  if (index < 0 || index >= 20) return;
  const Nota* mel = melodias[index];
  int size = tamanos[index];
  for (int i = 0; i < size; i++) {
    tone(BUZZER_PIN, mel[i].frecuencia, mel[i].duracion);
    delay(mel[i].duracion * 1.3);
    noTone(BUZZER_PIN);
  }
}

void siguienteMelodia() {
  melodiaActual++;
  if (melodiaActual >= 20) melodiaActual = 0;
  Serial.print("Reproduciendo siguiente melodía (");
  Serial.print(melodiaActual + 1);
  Serial.println(")");
  reproducirMelodia(melodiaActual);
}

void anteriorMelodia() {
  melodiaActual--;
  if (melodiaActual < 0) melodiaActual = 19;
  Serial.print("Reproduciendo anterior melodía (");
  Serial.print(melodiaActual + 1);
  Serial.println(")");
  reproducirMelodia(melodiaActual);
}

#include <Arduino.h>

#define BUZZER_PIN_1 7
#define BUZZER_PIN_2 8

struct Nota { int f; int d; };

extern void imprimirTiempo(const String &nombre, unsigned long dt);
extern bool tecladoEnUso;

volatile bool melodiaDetener = false;

void detenerMelodia() {
  melodiaDetener = true;
  noTone(BUZZER_PIN_1);
  noTone(BUZZER_PIN_2);
}

const Nota mario[] = {
  660,150,660,150,660,150,510,150,660,150,770,150,380,150,
  510,150,380,150,320,150,440,150,480,150,450,150,430,150,
  380,200,660,150,760,150,860,150,700,150,760,150,660,150,
  520,150,580,150,500,150,440,150,660,150,760,150,860,150,
  700,150,760,150,660,150,520,150,580,150,500,150,
  660,150,660,150,660,150,510,150,660,150,770,150,380,150,
  510,150,380,150,320,150,440,150,480,150,450,150,430,150
};

const Nota tetrisA[] = {
  659,300,494,150,523,150,587,300,523,150,494,150,440,300,
  440,150,523,150,659,300,587,150,523,150,494,300,
  494,150,523,150,587,300,659,300,523,300,440,300,
  659,300,494,150,523,150,587,300,523,150,494,150,440,300
};

const Nota starwars[] = {
  440,500,440,500,440,500,349,350,523,150,440,500,349,350,523,150,440,650,
  659,500,659,500,659,500,698,350,523,200,415,300,349,350,523,150,440,600,
  440,500,440,500,440,500,349,350,523,150,440,500,349,350,523,150,440,650
};

const Nota harrypotter[] = {
  659,400,494,200,523,200,587,400,587,200,523,200,494,600,
  659,400,494,200,523,200,587,600,698,400,659,200,587,200,523,200,494,200,523,200,587,800,
  659,400,494,200,523,200,587,400,587,200,523,200,494,600
};

const Nota mission[] = {
  659,200,659,200,659,200,523,200,587,200,622,400,
  659,200,659,200,659,200,523,200,587,200,622,400,
  659,200,784,200,740,200,698,200,659,200,587,200,554,200,523,200,
  659,200,659,200,659,200,523,200,587,200,622,400
};

const Nota pirates[] = {
  523,300,659,300,698,300,784,300,880,600,784,300,698,300,659,300,
  523,300,659,300,698,300,784,300,880,600,784,300,698,300,659,300,
  523,300,659,300,698,300,784,300,880,600
};

const Nota zelda[] = {
  784,250,784,250,988,250,1174,250,1318,300,1174,200,988,200,
  784,250,784,250,988,250,1174,300,1318,350,
  988,300,784,250,988,250,1174,250,1318,300,1174,200,988,200
};

const Nota titanic[] = {
  392,400,440,400,494,500,494,300,440,300,392,500,
  392,300,440,300,494,500,440,300,392,300,349,600,
  392,400,440,400,494,500,440,400,392,300,349,500
};

const Nota got[] = {
  392,400,523,400,587,400,523,400,392,400,349,400,
  392,400,523,400,587,400,523,400,392,400,349,400,
  392,400,523,400,587,400,523,400
};

const Nota nokia[] = {
  1318,350,1174,200,1046,350,587,350,659,350,784,350,1046,350,784,350,659,350,
  1318,350,1174,200,1046,350,587,350
};

const Nota llorandoSeFue[] = {
  440,300,494,300,523,300,494,300,440,350,392,350,
  392,300,440,300,494,300,523,300,494,350,440,350,
  494,300,523,300,587,300,523,300,494,350,440,350,
  440,300,494,300,523,300,494,300,440,350,392,350,
  440,300,494,300,523,300,494,300,440,350,392,350,
  392,300,440,300,494,300,523,300,494,350,440,350
};

const Nota* melodias[] = {
  mario, tetrisA, starwars, harrypotter, mission,
  pirates, zelda, titanic, got, nokia, llorandoSeFue
};

const int tamanos[] = {
  sizeof(mario)/sizeof(Nota), sizeof(tetrisA)/sizeof(Nota), sizeof(starwars)/sizeof(Nota),
  sizeof(harrypotter)/sizeof(Nota), sizeof(mission)/sizeof(Nota), sizeof(pirates)/sizeof(Nota),
  sizeof(zelda)/sizeof(Nota), sizeof(titanic)/sizeof(Nota), sizeof(got)/sizeof(Nota),
  sizeof(nokia)/sizeof(Nota), sizeof(llorandoSeFue)/sizeof(Nota)
};

int melodiaActual = 0;

void Melodias_begin() {
  pinMode(BUZZER_PIN_1, OUTPUT);
  pinMode(BUZZER_PIN_2, OUTPUT);
}

void reproducirMelodia(int index) {
  if (index < 0 || index >= 11) return;

  melodiaDetener = false;
  unsigned long t0 = micros();

  const Nota* mel = melodias[index];
  int n = tamanos[index];

  for (int i = 0; i < n; i++) {
    if (melodiaDetener) break;

    int f1 = mel[i].f;
    int f2 = mel[i].f * 0.66;

    tone(BUZZER_PIN_1, f1, mel[i].d);
    tone(BUZZER_PIN_2, f2, mel[i].d);

    unsigned long t = millis();
    while (millis() - t < mel[i].d) {
      if (melodiaDetener) break;
    }

    noTone(BUZZER_PIN_1);
    noTone(BUZZER_PIN_2);

    if (melodiaDetener) break;

    delay(mel[i].d * 0.25);
  }

  noTone(BUZZER_PIN_1);
  noTone(BUZZER_PIN_2);

  unsigned long dt = micros() - t0;
  imprimirTiempo("MELODIA", dt);
}

void siguienteMelodia() {
  detenerMelodia();
  melodiaActual++;
  if (melodiaActual >= 11) melodiaActual = 0;
  reproducirMelodia(melodiaActual);
}

void anteriorMelodia() {
  detenerMelodia();
  melodiaActual--;
  if (melodiaActual < 0) melodiaActual = 10;
  reproducirMelodia(melodiaActual);
}

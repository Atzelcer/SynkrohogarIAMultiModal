#include <Arduino.h>
#include <FastLED.h>
#include <Servo.h>

extern CRGB t1[];
extern CRGB t2[];
extern int N1;
extern int N2;

extern Servo s1;
extern Servo s2;

extern int l1;
extern int l2;

extern bool buzzersMuted;

extern void apagarUltra();

bool bypassESP32 = false;

bool gestoSiguiente = false;
bool gestoAnterior  = false;
bool cambioAutomatico = false;

int modoActual = 0;

#define NORMAL 0
#define SUPER  1
#define ULTRA  2
#define AURORA 3

void apagarAurora() {
    for (int i = 0; i < N1; i++) t1[i] = CRGB::Black;
    for (int i = 0; i < N2; i++) t2[i] = CRGB::Black;
    FastLED.show();
}

void centrarServos() {
    if (!s1.attached()) s1.attach(34);
    if (!s2.attached()) s2.attach(35);
    s1.write(90);
    s2.write(90);
    delay(60);
    s1.detach();
    s2.detach();
}

void apagarLasers() {
    digitalWrite(l1, LOW);
    digitalWrite(l2, LOW);
}

void cambiarModo(int nuevoModo) {

    gestoSiguiente = false;
    gestoAnterior  = false;

    if (nuevoModo == NORMAL) {

        apagarUltra();
        apagarAurora();
        apagarLasers();
        centrarServos();

        modoActual = NORMAL;
        bypassESP32 = false;
        return;
    }

    if (modoActual != NORMAL && !bypassESP32) {
        return;
    }

    if (modoActual == ULTRA) {
        apagarUltra();
    }

    if (modoActual == AURORA) {
        apagarAurora();
    }

    modoActual = nuevoModo;
    bypassESP32 = false;
}

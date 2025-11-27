#include <Arduino.h>

#define LED1 3
#define LED2 4
#define LED3 5
#define LED4 11

extern bool tecladoEnUso;
extern void imprimirTiempo(const String &nombre, unsigned long dt);

void LEDS_begin() {
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    apagarTodosLEDs();
}

void encenderTodosLEDs() {
    if (tecladoEnUso) return;

    unsigned long t0 = micros();

    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);

    unsigned long dt = micros() - t0;
    imprimirTiempo("LED_ON", dt);
}

void apagarTodosLEDs() {
    if (tecladoEnUso) return;

    unsigned long t0 = micros();

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);

    unsigned long dt = micros() - t0;
    imprimirTiempo("LED_OFF", dt);
}

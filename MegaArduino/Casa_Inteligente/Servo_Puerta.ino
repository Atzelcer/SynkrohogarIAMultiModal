#include <Servo.h>
#include <Arduino.h>

extern bool tecladoEnUso;

Servo servo1;
Servo servo2;

#define SERVO1_PIN 9
#define SERVO2_PIN 10

extern void mostrarEnLCD(const String &line1, const String &line2);
extern void setBuzzersMuted(bool mute);
extern void mostrarPanelPrincipal();
extern void imprimirTiempo(const String &nombre, unsigned long dt);

void Servo_begin() {
    servo1.attach(SERVO1_PIN);
    servo2.attach(SERVO2_PIN);
    servo1.write(90);
    servo2.write(90);
}

void abrirPuerta(unsigned long ms) {

    unsigned long t0 = micros();

    if (!tecladoEnUso) mostrarEnLCD("Puerta:", "Abriendo");
    setBuzzersMuted(true);

    for (int a = 90; a <= 180; a += 3) {
        servo1.write(a);
        servo2.write(a);
        delay(12);
    }

    delay(ms);

    if (!tecladoEnUso) mostrarEnLCD("Puerta:", "Cerrando");

    for (int a = 180; a >= 90; a -= 3) {
        servo1.write(a);
        servo2.write(a);
        delay(12);
    }

    servo1.write(90);
    servo2.write(90);

    setBuzzersMuted(false);

    if (!tecladoEnUso) mostrarPanelPrincipal();

    unsigned long dt = micros() - t0;
    imprimirTiempo("PUERTA", dt);
}

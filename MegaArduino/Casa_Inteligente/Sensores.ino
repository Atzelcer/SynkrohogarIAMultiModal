#include <Arduino.h>

#define PIR1_EXTERIOR 30
#define PIR2_INTERIOR 31
#define PIR_POWER_PIN 5

extern void mostrarEnLCD(const String &line1, const String &line2);
extern void playAlertPIR1();
extern void playAlertPIR2();
extern void playPIRSirenaFuerte();
extern void stopAlerts();
extern bool sistemaActivo;
extern bool tecladoEnUso;
extern void mostrarPanelPrincipal();

unsigned long ultimaLectura = 0;
const unsigned long INTERVALO_LECTURA = 1000;

bool alarmaPIRActiva = false;

void SensoresPIR_begin() {
    pinMode(PIR1_EXTERIOR, INPUT);
    pinMode(PIR2_INTERIOR, INPUT);
    pinMode(PIR_POWER_PIN, OUTPUT);
    pinMode(25, INPUT);
}

void verificarPIR() {
    if (!sistemaActivo) return;

    int pir1 = digitalRead(PIR1_EXTERIOR);
    int pir2 = digitalRead(PIR2_INTERIOR);

    bool alertaActiva = (pir1 == HIGH || pir2 == HIGH);

    if (tecladoEnUso) return;

    if (pir1 == HIGH && pir2 == LOW) {

        unsigned long t0 = micros();
        playAlertPIR1();
        if (!alarmaPIRActiva) playPIRSirenaFuerte();
        unsigned long dt = micros() - t0;

        float ms = dt / 1000.0;

        Serial.print("PIR1: ");
        Serial.print(ms, 3);
        Serial.println(" ms");

        Serial.println("ALERTA: PIR1 EXTERIOR");

        mostrarEnLCD("ALERTA PIR1", String(ms, 3) + " ms");

        alarmaPIRActiva = true;
        return;
    }

    if (pir2 == HIGH && pir1 == LOW) {

        unsigned long t0 = micros();
        playAlertPIR2();
        if (!alarmaPIRActiva) playPIRSirenaFuerte();
        unsigned long dt = micros() - t0;

        float ms = dt / 1000.0;

        Serial.print("PIR2: ");
        Serial.print(ms, 3);
        Serial.println(" ms");

        Serial.println("ALERTA: PIR2 INTERIOR");

        mostrarEnLCD("ALERTA PIR2", String(ms, 3) + " ms");

        alarmaPIRActiva = true;
        return;
    }

    if (alertaActiva && alarmaPIRActiva) {
        mostrarEnLCD("ALERTA PIR", "Sirena activa");
        return;
    }

    if (!alertaActiva && alarmaPIRActiva) {
        alarmaPIRActiva = false;
        stopAlerts();
        mostrarPanelPrincipal();
        return;
    }

    if (!alertaActiva && !alarmaPIRActiva) {
        if (millis() - ultimaLectura >= INTERVALO_LECTURA) {
            ultimaLectura = millis();
            Serial.print("PIR1: ");
            Serial.print(pir1 == HIGH ? "MOVIMIENTO   " : "Inactivo   ");
            Serial.print("| PIR2: ");
            Serial.println(pir2 == HIGH ? "MOVIMIENTO" : "Inactivo");
        }
    }
}

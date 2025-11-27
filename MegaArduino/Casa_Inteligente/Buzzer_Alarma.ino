#include <Arduino.h>

#define BUZZER_ALARMA 6
#define BUZZER_OK_1 7
#define BUZZER_OK_2 8

extern void imprimirTiempo(const String &nombre, unsigned long dt);

bool buzzersMuted = false;

void Buzzer_begin() {
  pinMode(BUZZER_ALARMA, OUTPUT);
  pinMode(BUZZER_OK_1, OUTPUT);
  pinMode(BUZZER_OK_2, OUTPUT);
  digitalWrite(BUZZER_ALARMA, LOW);
}

void setBuzzersMuted(bool mute) {
  unsigned long t0 = micros();
  buzzersMuted = mute;
  noTone(BUZZER_ALARMA);
  noTone(BUZZER_OK_1);
  noTone(BUZZER_OK_2);
  unsigned long dt = micros() - t0;
  imprimirTiempo("BUZZER_MUTE", dt);
}

void playError() {
  unsigned long t0 = micros();
  if (!buzzersMuted) {
    unsigned long inicio = millis();
    while (millis() - inicio < 4000) {
      for (int f = 800; f <= 2000; f += 40) {
        tone(BUZZER_ALARMA, f, 20);
        delay(20);
        if (millis() - inicio >= 4000) break;
      }
      for (int f = 2000; f >= 800; f -= 40) {
        tone(BUZZER_ALARMA, f, 20);
        delay(20);
        if (millis() - inicio >= 4000) break;
      }
    }
  }
  unsigned long dt = micros() - t0;
  imprimirTiempo("BUZZER_ERROR", dt);
}

void playAccess() {
  unsigned long t0 = micros();
  if (!buzzersMuted) {
    tone(BUZZER_OK_1, 1200, 100);
    tone(BUZZER_OK_2, 800, 100);
    delay(120);

    tone(BUZZER_OK_1, 1500, 100);
    tone(BUZZER_OK_2, 1100, 100);
    delay(130);

    tone(BUZZER_OK_1, 1800, 160);
    tone(BUZZER_OK_2, 1400, 160);
  }
  unsigned long dt = micros() - t0;
  imprimirTiempo("BUZZER_ACCESS", dt);
}

void playActivationSound() {
  unsigned long t0 = micros();
  if (!buzzersMuted) {
    tone(BUZZER_OK_1, 1000, 120);
    tone(BUZZER_OK_2, 700, 120);
    delay(140);

    tone(BUZZER_OK_1, 1500, 180);
    tone(BUZZER_OK_2, 1000, 180);
  }
  unsigned long dt = micros() - t0;
  imprimirTiempo("BUZZER_ACTIVAR", dt);
}

void playDeactivationSound() {
  unsigned long t0 = micros();
  if (!buzzersMuted) {
    tone(BUZZER_OK_1, 1500, 120);
    tone(BUZZER_OK_2, 1000, 120);
    delay(140);

    tone(BUZZER_OK_1, 900, 180);
    tone(BUZZER_OK_2, 600, 180);
  }
  unsigned long dt = micros() - t0;
  imprimirTiempo("BUZZER_DESACTIVAR", dt);
}

void playAlertPIR1() {
  unsigned long t0 = micros();
  if (!buzzersMuted) {
    tone(BUZZER_ALARMA, 1800, 140);
    delay(150);
    tone(BUZZER_ALARMA, 2600, 140);
    delay(150);
  }
  unsigned long dt = micros() - t0;
  imprimirTiempo("BUZZER_PIR1", dt);
}

void playAlertPIR2() {
  unsigned long t0 = micros();
  if (!buzzersMuted) {
    tone(BUZZER_ALARMA, 1600, 140);
    delay(150);
    tone(BUZZER_ALARMA, 2400, 140);
    delay(150);
  }
  unsigned long dt = micros() - t0;
  imprimirTiempo("BUZZER_PIR2", dt);
}

void playPIRSirenaFuerte() {
  unsigned long t0 = micros();
  if (!buzzersMuted) {
    tone(BUZZER_ALARMA, 3200, 6000);
  }
  unsigned long dt = micros() - t0;
  imprimirTiempo("BUZZER_SIRENA", dt);
}

void stopAlerts() {
  unsigned long t0 = micros();
  noTone(BUZZER_OK_1);
  noTone(BUZZER_OK_2);
  noTone(BUZZER_ALARMA);
  unsigned long dt = micros() - t0;
  imprimirTiempo("BUZZER_STOP", dt);
}

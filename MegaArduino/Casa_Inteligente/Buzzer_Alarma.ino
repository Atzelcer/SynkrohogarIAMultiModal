#include <Arduino.h>

#define BUZZER_ALARMA 8  // Sirena Fuerte (ALERTA MAYOR PIR)
#define BUZZER_PIR1 6    // Buzzer para Alerta PIR 1 (Pin 6)
#define BUZZER_PIR2 7    // Buzzer para Alerta PIR 2 (Pin 7)

bool buzzersMuted = false;
const long ALARM_DURATION_MS = 7000;

void Buzzer_begin() {
    pinMode(BUZZER_ALARMA, OUTPUT);
    pinMode(BUZZER_PIR1, OUTPUT);
    pinMode(BUZZER_PIR2, OUTPUT);
    digitalWrite(BUZZER_ALARMA, LOW);
}

void setBuzzersMuted(bool mute) {
    buzzersMuted = mute;
    if (mute) {
        noTone(BUZZER_ALARMA);
        noTone(BUZZER_PIR1);
        noTone(BUZZER_PIR2);
    }
}

void playError() {
    if (buzzersMuted) return;
    tone(BUZZER_PIR1, 900, 300); 
}

void playAccess() {
    if (buzzersMuted) return;
    int n1[] = {1200, 1500, 1800};
    for (int i = 0; i < 3; i++) {
        tone(BUZZER_PIR1, n1[i], 80);
        delay(100);
    }
    noTone(BUZZER_PIR1);
}

void playActivationSound() {
    tone(BUZZER_PIR1, 1000, 100); 
    delay(100);
    tone(BUZZER_PIR1, 1500, 150); 
    delay(150);
}

void playDeactivationSound() {
    tone(BUZZER_PIR1, 1500, 100); 
    delay(100);
    tone(BUZZER_PIR1, 1000, 150); 
    delay(150);
}

// Alerta PIR 1 (Pin 6): Tono Agudo Sostenido
void playAlertPIR1() {
    tone(BUZZER_PIR1, 3500); 
    noTone(BUZZER_PIR2); 
}

// Alerta PIR 2 (Pin 7): Tono Grave Sostenido
void playAlertPIR2() {
    tone(BUZZER_PIR2, 1000); 
    noTone(BUZZER_PIR1); 
}

void playPIRSirenaFuerte() {
    tone(BUZZER_ALARMA, 3000, ALARM_DURATION_MS); 
    noTone(BUZZER_PIR1); 
    noTone(BUZZER_PIR2); 
}

void stopAlerts() {
    noTone(BUZZER_PIR1);
    noTone(BUZZER_PIR2);
    noTone(BUZZER_ALARMA); 
}
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

// Variables de estado
unsigned long ultimaLectura = 0;
const unsigned long INTERVALO_LECTURA = 1000; 
bool alarmaPIRActiva = false;

void SensoresPIR_begin() {
    pinMode(PIR1_EXTERIOR, INPUT);
    pinMode(PIR2_INTERIOR, INPUT);
    pinMode(PIR_POWER_PIN, OUTPUT);
    pinMode(25, INPUT); 
    Serial.println("Módulos PIR listos.");
}

void verificarPIR() {
    if (!sistemaActivo) return;

    int pir1 = digitalRead(PIR1_EXTERIOR);
    int pir2 = digitalRead(PIR2_INTERIOR);
    
    bool alertaActiva = pir1 == HIGH || pir2 == HIGH;

    if (tecladoEnUso) return; 

    // Imprimir estado y actualizar LCD (solo si no hay alarma fuerte activa)
    if (millis() - ultimaLectura >= INTERVALO_LECTURA) {
        Serial.print("PIR1 Exterior: ");
        Serial.print(pir1 == HIGH ? "MOVIMIENTO " : "Inactivo   ");
        Serial.print("| PIR2 Interior: ");
        Serial.println(pir2 == HIGH ? "MOVIMIENTO" : "Inactivo");
        ultimaLectura = millis();
        
        if (!alertaActiva && !alarmaPIRActiva) {
            String line2_pir = "P1:" + String(pir1) + " P2:" + String(pir2);
            mostrarEnLCD("Modo Super (SEGURO)", line2_pir); 
        }
    }

    // --- Lógica de Alerta PIR ---
    
    if (pir1 == HIGH && pir2 == LOW) {
        playAlertPIR1(); // Tono AGUDO Pin 6
        Serial.println("!!! ALERTA: MOVIMIENTO DETECTADO EN PIR1 EXTERIOR !!! ");
        mostrarEnLCD("!!! ALERTA MOV. !!!", "PIR 1 - EXTERIOR"); 
        
        // Si no hay alarma fuerte activa, se dispara la sirena principal
        if (!alarmaPIRActiva) {
            alarmaPIRActiva = true;
            playPIRSirenaFuerte(); 
        }
    } else if (pir2 == HIGH && pir1 == LOW) {
        playAlertPIR2(); // Tono GRAVE Pin 7
        Serial.println("!!! ALERTA: MOVIMIENTO DETECTADO EN PIR2 INTERIOR !!! ");
        mostrarEnLCD("!!! ALERTA MOV. !!!", "PIR 2 - INTERIOR"); 
        
        if (!alarmaPIRActiva) {
            alarmaPIRActiva = true;
            playPIRSirenaFuerte(); 
        }
    } else if (alertaActiva && alarmaPIRActiva) {
        // Mantener el estado de alarma visual si la sirena fuerte está en curso
         mostrarEnLCD("!!! ALERTA MOVIMIENTO !!!", "Sirena PIR ACTIVA"); 
    } else {
        if (alarmaPIRActiva) {
             // Espera a que la alarma termine y el sensor se calme
             if (digitalRead(PIR1_EXTERIOR) == LOW && digitalRead(PIR2_INTERIOR) == LOW) { 
                 alarmaPIRActiva = false;
                 stopAlerts();
                 mostrarPanelPrincipal(); 
             }
        } else {
            stopAlerts(); 
        }
    }
}
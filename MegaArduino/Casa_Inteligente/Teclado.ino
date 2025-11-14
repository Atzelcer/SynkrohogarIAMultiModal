#include <Keypad.h>
#include <Arduino.h>

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};
byte rowPins[ROWS] = {22, 23, 24, 25}; 
byte colPins[COLS] = {26, 27, 28}; 
Keypad teclado = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String claveIngresada = "";
const String CLAVE_ACTIVAR      = "1111"; 
const String CLAVE_DESACTIVAR   = "0000"; 
const String CLAVE_PUERTA       = "1234";

bool sistemaActivo = false;
bool tecladoEnUso = false; 

extern void mostrarEnLCD(const String &line1, const String &line2);
extern void playActivationSound();
extern void playDeactivationSound();
extern void playError();
extern void playAccess();
extern void abrirPuerta(unsigned long ms);
extern void stopAlerts();
extern void mostrarPanelPrincipal();

#define PIR_POWER_PIN 5  

void Teclado_begin() {
}

void activarSistema() {
    digitalWrite(PIR_POWER_PIN, HIGH); 
    sistemaActivo = true;
    tecladoEnUso = false; 
    
    playActivationSound(); 
    
    Serial.println("====================================");
    Serial.println(">> MODO SUPER ACTIVADO (1111#) <<");
    Serial.println("====================================");
    
    mostrarEnLCD("Clave OK!", "Modo Super ACTIVADO"); 
    delay(1500);
    mostrarPanelPrincipal(); 
}

void desactivarSistema() {
    sistemaActivo = false;
    digitalWrite(PIR_POWER_PIN, LOW); 
    
    stopAlerts();
    
    playDeactivationSound(); 
    tecladoEnUso = false; 
    
    Serial.println("====================================");
    Serial.println(">> MODO NORMAL DESACTIVADO (0000#) <<");
    Serial.println(">> Alimentación PIR APAGADA. <<");
    Serial.println("====================================");
    
    mostrarEnLCD("Clave OK!", "Modo Normal (OFF)"); 
    delay(1500);
    mostrarPanelPrincipal(); 
}

void leerTeclado() {
    char tecla = teclado.getKey();
    if (!tecla) return;

    if (tecla != '*' && tecla != '#') {
        tecladoEnUso = true;
        mostrarEnLCD("Clave:", claveIngresada + tecla + "*"); 
    }

    if (tecla == '#') {
        if (claveIngresada == CLAVE_ACTIVAR) {
            activarSistema();
        } else if (claveIngresada == CLAVE_DESACTIVAR) {
            desactivarSistema();
        } else if (claveIngresada == CLAVE_PUERTA) {
            mostrarEnLCD("Clave OK!", "Abriendo Puerta...");
            playAccess();
            abrirPuerta(5000);
            mostrarPanelPrincipal(); 
            tecladoEnUso = false;
        } else {
            Serial.println("Clave INCORRECTA.");
            playError();
            mostrarEnLCD("Clave INCORRECTA!", "ACCESO DENEGADO"); 
            delay(1500);
            mostrarPanelPrincipal(); 
            tecladoEnUso = false; 
        }
        claveIngresada = "";
        return;
    }

    if (tecla == '*') {
        claveIngresada = "";
        Serial.println("Clave borrada.");
        mostrarEnLCD("Clave borrada", "Ingrese de nuevo");
        tecladoEnUso = false; 
        delay(1000);
        mostrarPanelPrincipal(); 
        return;
    }

    if (claveIngresada.length() < 8) {
        claveIngresada += tecla;
        String msg1 = "Clave: " + claveIngresada;
        Serial.print(msg1);
        Serial.println("*");
    }
}
#include <Keypad.h>
#include <Arduino.h>
#include <FastLED.h>

extern void playActivationSound();
extern void playDeactivationSound();
extern void playError();
extern void playAccess();
extern void abrirPuerta(unsigned long ms);
extern void stopAlerts();
extern void cambiarModo(int nuevoModo);
extern void imprimirTiempo(const String &nombre, unsigned long dt);
extern void mostrarEnLCD(const String &l1, const String &l2);
extern void mostrarPanelPrincipal();

extern CRGB t1[];
extern CRGB t2[];
extern int N1;
extern int N2;

#define PIR_POWER_PIN 5

#define NORMAL 0
#define SUPER  1
#define ULTRA  2
#define AURORA 3

extern int modoActual;

bool sistemaActivo  = false;
bool tecladoEnUso   = false;

String claveIngresada = "";

// TECLADO
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {22,23,24,25};
byte colPins[COLS] = {26,27,28};

Keypad teclado = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void Teclado_begin() {}

// ========================================================
// ACTIVAR SISTEMA (SUPER)
// ========================================================
void activarSistema() {

    unsigned long t0 = micros();

    if (modoActual != NORMAL) {
        playError();
        imprimirTiempo("TECLADO_ERROR", micros() - t0);
        return;
    }

    digitalWrite(PIR_POWER_PIN, HIGH);
    sistemaActivo = true;

    playActivationSound();
    cambiarModo(SUPER);

    imprimirTiempo("TECLADO_SUPER", micros() - t0);
}

// ========================================================
// DESACTIVAR SISTEMA (NORMAL)
// ========================================================
void desactivarSistema() {

    unsigned long t0 = micros();

    sistemaActivo = false;

    digitalWrite(PIR_POWER_PIN, LOW);
    stopAlerts();
    playDeactivationSound();

    for (int i = 0; i < N1; i++) t1[i] = CRGB::Black;
    for (int i = 0; i < N2; i++) t2[i] = CRGB::Black;
    FastLED.show();

    cambiarModo(NORMAL);

    imprimirTiempo("TECLADO_NORMAL", micros() - t0);
}

// ========================================================
// LÓGICA PRINCIPAL DEL TECLADO
// ========================================================
void leerTeclado() {

    char t = teclado.getKey();
    if (!t) return;

    // --- DIGITOS ---
    if (t != '*' && t != '#') {

        tecladoEnUso = true;

        if (claveIngresada.length() < 8) {
            claveIngresada += t;

            // MOSTRAR EN LCD EN TIEMPO REAL
            mostrarEnLCD("Clave:", claveIngresada);
        }

        return;
    }

    // --- BORRAR ---
    if (t == '*') {
        claveIngresada = "";
        tecladoEnUso = true;
        mostrarEnLCD("Clave:", "");
        return;
    }

    // --- PROCESAR (#) ---
    if (t == '#') {

        unsigned long t0 = micros();

        if (claveIngresada == "1111") activarSistema();
        else if (claveIngresada == "0000") desactivarSistema();
        else if (claveIngresada == "1234") { playAccess(); abrirPuerta(5000); }
        else if (claveIngresada == "2222") {
            if (modoActual != NORMAL) playError();
            else cambiarModo(ULTRA);
        }
        else if (claveIngresada == "3333") {
            if (modoActual != NORMAL) playError();
            else cambiarModo(AURORA);
        }
        else {
            playError();
        }

        imprimirTiempo("TECLADO", micros() - t0);

        claveIngresada = "";
        tecladoEnUso = false;

        mostrarPanelPrincipal();
    }
}

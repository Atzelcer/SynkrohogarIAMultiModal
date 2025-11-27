#include <Arduino.h>
#define SERIAL_ESP Serial2

void LCD_begin();
void mostrarEnLCD(const String &line1, const String &line2 = "");
void mostrarPanelPrincipal();

void RFID_begin();
String RFID_readUIDIfPresent();

void Teclado_begin();
void leerTeclado();
void activarSistema();
void desactivarSistema();
extern bool sistemaActivo;
extern bool tecladoEnUso;

void Servo_begin();
void abrirPuerta(unsigned long ms = 5000);

void Buzzer_begin();
void playActivationSound();
void playDeactivationSound();
void playError();
void playAccess();

void SensoresPIR_begin();
void verificarPIR();

void Melodias_begin();
void siguienteMelodia();
void anteriorMelodia();
void reproducirMelodia(int index);
extern void detenerMelodia();

void LEDS_begin();
void encenderTodosLEDs();
void apagarTodosLEDs();

void EfectosEspeciales_begin();

extern int modoActual;
extern void cambiarModo(int nuevoModo);
extern void modoUltra();
extern void modoAurora();
extern bool gestoSiguiente;
extern bool gestoAnterior;
extern bool cambioAutomatico;

#define NORMAL 0
#define SUPER  1
#define ULTRA  2
#define AURORA 3

void imprimirTiempo(const String &nombre, unsigned long dt) {
  float seg = dt / 1000000.0;
  Serial.print(nombre);
  Serial.print(": ");
  Serial.print(seg, 3);
  Serial.println(" seg");
  mostrarEnLCD(nombre, String(seg, 3) + " seg");
}

void setup() {
  Serial.begin(115200);
  SERIAL_ESP.begin(115200);

  LCD_begin();
  Buzzer_begin();
  Servo_begin();
  RFID_begin();
  Teclado_begin();
  SensoresPIR_begin();
  Melodias_begin();
  LEDS_begin();
  EfectosEspeciales_begin();

  cambiarModo(NORMAL);
  mostrarPanelPrincipal();
  SERIAL_ESP.println("ESP_SYNC|READY");
}

void procesarComandoESP32(String msg) {

  leerTeclado();
  RFID_readUIDIfPresent();
  detenerMelodia();

  if (msg == "CMD|OPEN_DOOR") {
    unsigned long t0 = micros();
    abrirPuerta(3000);
    imprimirTiempo("PUERTA", micros() - t0);
    SERIAL_ESP.println("ACK|OPEN_DOOR");
    return;
  }

  // ============================================================
  // SUPER ON → EXACTO AL TECLADO (1111)
  // ============================================================
  if (msg == "CMD|SUPER_ON") {
    unsigned long t0 = micros();
    cambioAutomatico = false;

    activarSistema();  // ← PRIMERO, igual que el teclado
    cambiarModo(SUPER);

    imprimirTiempo("SUPER ON", micros() - t0);
    SERIAL_ESP.println("ACK|SUPER_ON");
    return;
  }

  if (msg == "CMD|SUPER_OFF") {
    unsigned long t0 = micros();
    cambiarModo(NORMAL);
    desactivarSistema();
    imprimirTiempo("SUPER OFF", micros() - t0);
    SERIAL_ESP.println("ACK|SUPER_OFF");
    return;
  }

  // ============================================================
  // ULTRA
  // ============================================================
  if (msg == "CMD|ULTRA_ON") {
    if (modoActual != NORMAL) {
      SERIAL_ESP.println("ERR|NO_NORMAL");
      return;
    }
    unsigned long t0 = micros();
    cambioAutomatico = false;
    cambiarModo(ULTRA);
    imprimirTiempo("ULTRA ON", micros() - t0);
    SERIAL_ESP.println("ACK|ULTRA_ON");
    return;
  }

  // ============================================================
  // AURORA
  // ============================================================
  if (msg == "CMD|AURORA_ON") {
    if (modoActual != NORMAL) {
      SERIAL_ESP.println("ERR|NO_NORMAL");
      return;
    }
    unsigned long t0 = micros();
    cambioAutomatico = false;
    cambiarModo(AURORA);
    imprimirTiempo("AURORA ON", micros() - t0);
    SERIAL_ESP.println("ACK|AURORA_ON");
    return;
  }

if (msg == "CMD|NORMAL_ON") {
    unsigned long t0 = micros();

    desactivarSistema();  

    cambiarModo(NORMAL);
    mostrarPanelPrincipal();

    imprimirTiempo("NORMAL", micros() - t0);
    SERIAL_ESP.println("ACK|NORMAL_ON");
    return;
}


  // ============================================================
  // LEDS
  // ============================================================
  if (msg == "CMD|LED_ON") {
    unsigned long t0 = micros();
    encenderTodosLEDs();
    imprimirTiempo("LED ON", micros() - t0);
    SERIAL_ESP.println("ACK|LED_ON");
    return;
  }

  if (msg == "CMD|LED_OFF") {
    unsigned long t0 = micros();
    apagarTodosLEDs();
    imprimirTiempo("LED OFF", micros() - t0);
    SERIAL_ESP.println("ACK|LED_OFF");
    return;
  }

  // ============================================================
  // MÚSICA
  // ============================================================
  if (msg == "CMD|CHANGE_MUSIC") {
    siguienteMelodia();
    SERIAL_ESP.println("ACK|CHANGE_MUSIC");
    return;
  }

  if (msg == "CMD|PREV_MUSIC") {
    anteriorMelodia();
    SERIAL_ESP.println("ACK|PREV_MUSIC");
    return;
  }

  if (msg == "CMD|PLAY_MUSIC") {
    reproducirMelodia(0);
    SERIAL_ESP.println("ACK|PLAY_MUSIC");
    return;
  }

  // ============================================================
  // AURORA GESTOS
  // ============================================================
  if (msg == "CMD|AURORA_NEXT") {
    gestoSiguiente = true;
    SERIAL_ESP.println("ACK|AURORA_NEXT");
    return;
  }

  if (msg == "CMD|AURORA_PREV") {
    gestoAnterior = true;
    SERIAL_ESP.println("ACK|AURORA_PREV");
    return;
  }

  SERIAL_ESP.println("ERR|UNKNOWN_CMD");
}

void loop() {

  leerTeclado();
  RFID_readUIDIfPresent();
  verificarPIR();

  if (modoActual == ULTRA) {
    detenerMelodia();
    unsigned long t0 = micros();
    modoUltra();
    imprimirTiempo("ULTRA", micros() - t0);
  }

  if (modoActual == AURORA) {
    detenerMelodia();
    unsigned long t0 = micros();
    modoAurora();
    imprimirTiempo("AURORA", micros() - t0);
  }

  if (SERIAL_ESP.available()) {
    String msg = SERIAL_ESP.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) procesarComandoESP32(msg);
  }

  if (Serial.available()) {
    SERIAL_ESP.write(Serial.read());
  }

  delay(20);
}

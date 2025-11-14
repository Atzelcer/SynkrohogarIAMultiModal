#include <Arduino.h>
#define SERIAL_ESP Serial2  // Comunicación con ESP32-CAM

// ----------------- DECLARACIONES DE MÓDULOS -----------------
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
void setBuzzersMuted(bool mute);

void Buzzer_begin();
void playActivationSound();
void playDeactivationSound();
void playError();
void playAccess();
void stopAlerts();

void SensoresPIR_begin();
void verificarPIR();

void Melodias_begin();
void siguienteMelodia();
void anteriorMelodia();
void reproducirMelodia(int index);

// --- Módulo LEDs ---
void LEDS_begin();
void encenderTodosLEDs();
void apagarTodosLEDs();

// ----------------- VARIABLES GLOBALES -----------------
unsigned long lastStatusSend = 0;

// ----------------- SETUP -----------------
void setup() {
  Serial.begin(115200);       // Comunicación con PC
  SERIAL_ESP.begin(115200);   // Comunicación con ESP32-CAM

  LCD_begin();
  Buzzer_begin();
  Servo_begin();
  RFID_begin();
  Teclado_begin();
  SensoresPIR_begin();
  Melodias_begin();
  LEDS_begin(); // inicializa LEDs

  Serial.println("Sistema SynkroHogar - Módulo Central Iniciado.");
  desactivarSistema();
  delay(1500);
  mostrarPanelPrincipal();

  SERIAL_ESP.println("ESP_SYNC|READY");
  Serial.println("Esperando comandos desde ESP32-CAM...");
}

// ----------------- LOOP PRINCIPAL -----------------
void loop() {
  leerTeclado();
  RFID_readUIDIfPresent();
  verificarPIR();

  // --------- RECEPCIÓN DE COMANDOS DESDE ESP32 ---------
  if (SERIAL_ESP.available()) {
    String msg = SERIAL_ESP.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) {
      Serial.print("[Desde ESP32] ");
      Serial.println(msg);

      if (msg == "CMD|OPEN_DOOR") {
        abrirPuerta(3000);
        SERIAL_ESP.println("ACK|OPEN_DOOR");
      } 
      else if (msg == "CMD|SUPER_ON") {
        activarSistema();
        SERIAL_ESP.println("ACK|SUPER_ON");
      } 
      else if (msg == "CMD|SUPER_OFF") {
        desactivarSistema();
        SERIAL_ESP.println("ACK|SUPER_OFF");
      } 
      else if (msg == "CMD|CHANGE_MUSIC") {
        siguienteMelodia();
        SERIAL_ESP.println("ACK|CHANGE_MUSIC");
      }
      else if (msg == "CMD|PREV_MUSIC") {
        anteriorMelodia();
        SERIAL_ESP.println("ACK|PREV_MUSIC");
      }
      else if (msg == "CMD|PLAY_MUSIC") {
        reproducirMelodia(0);
        SERIAL_ESP.println("ACK|PLAY_MUSIC");
      }
      else if (msg == "CMD|LED_ON") {
        encenderTodosLEDs();
        SERIAL_ESP.println("ACK|LED_ON");
      }
      else if (msg == "CMD|LED_OFF") {
        apagarTodosLEDs();
        SERIAL_ESP.println("ACK|LED_OFF");
      }else {
        SERIAL_ESP.println("ERR|UNKNOWN_CMD");
      }
    }
  }

  // --------- DEPURACIÓN OPCIONAL DESDE PC ---------
  if (Serial.available()) {
    SERIAL_ESP.write(Serial.read());
  }

  // --------- ENVÍO DE ESTADO AL ESP32 ---------
  unsigned long now = millis();
  if (now - lastStatusSend >= 2000) {
    lastStatusSend = now;
    String estado = String("STATUS|SYS=") + (sistemaActivo ? "ON" : "OFF");
    SERIAL_ESP.println(estado);
    Serial.println("[Mega→ESP32] " + estado);
  }

  delay(30);
}

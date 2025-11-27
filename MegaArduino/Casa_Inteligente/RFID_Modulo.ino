#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>

#define SS_PIN 40
#define RST_PIN 41

MFRC522 rfid(SS_PIN, RST_PIN);

const String VALID_UID_ACCESO = "E3183E2E";

extern void mostrarEnLCD(const String &line1, const String &line2);
extern void playAccess();
extern void playError();
extern void abrirPuerta(unsigned long ms);
extern void mostrarPanelPrincipal();
extern bool tecladoEnUso;
extern void imprimirTiempo(const String &nombre, unsigned long dt);

void RFID_begin() {
    SPI.begin();
    rfid.PCD_Init();
    Serial.println("RFID listo");
}

String RFID_readUIDIfPresent() {

    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return "";
    }

    unsigned long t0 = micros();

    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) uid += "0";
        uid += String(rfid.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();

    Serial.print("RFID UID leído: ");
    Serial.println(uid);

    // -----------------------------------------------------------
    // UID CORRECTO
    // -----------------------------------------------------------
    if (uid == VALID_UID_ACCESO) {

        Serial.println("RFID correcto → Acceso permitido.");

        if (!tecladoEnUso)
            mostrarEnLCD("RFID OK", "Abriendo puerta...");

        playAccess();
        abrirPuerta(3000);

        if (!tecladoEnUso)
            mostrarPanelPrincipal();
    }

    // -----------------------------------------------------------
    // UID INCORRECTO
    // -----------------------------------------------------------
    else {

        Serial.println("RFID incorrecto → Acceso DENEGADO.");

        if (!tecladoEnUso)
            mostrarEnLCD("RFID INVALIDO", "ACCESO DENEGADO");

        playError();       // Usa la sirena de 4 segundos

        delay(1500);

        if (!tecladoEnUso)
            mostrarPanelPrincipal();
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    unsigned long dt = micros() - t0;
    imprimirTiempo("RFID", dt);

    return uid;
}

void RFID_halt() {
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

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


void RFID_begin() {
    SPI.begin();
    rfid.PCD_Init();
    Serial.println("Módulo RFID listo.");
}

String RFID_readUIDIfPresent() {
    if (tecladoEnUso) return "";
    
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return "";
    }
    
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) uid += "0";
        uid += String(rfid.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();
    
    Serial.print("Tarjeta detectada - UID: ");
    Serial.println(uid);

    if (uid == VALID_UID_ACCESO) { 
        Serial.println(">> ACCESO RFID PERMITIDO");
        mostrarEnLCD("RFID: Clave OK", "Abriendo Puerta...");
        playAccess();
        abrirPuerta(3000); 
        mostrarPanelPrincipal();
        
    } else {
        Serial.println("!! ACCESO RFID DENEGADO");
        playError();
        mostrarEnLCD("RFID: INCORRECTO!", "ACCESO DENEGADO");
        delay(1500);
        mostrarPanelPrincipal();
    }
    
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    
    return uid;
}

void RFID_halt() {
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}
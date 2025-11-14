#include <Arduino.h>

#define LED1 3
#define LED2 4
#define LED3 5
#define LED4 11

void LEDS_begin() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  apagarTodosLEDs();
}

void encenderTodosLEDs() {
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  Serial.println("Luces encendidas.");
}

void apagarTodosLEDs() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  Serial.println("Luces apagadas.");
}

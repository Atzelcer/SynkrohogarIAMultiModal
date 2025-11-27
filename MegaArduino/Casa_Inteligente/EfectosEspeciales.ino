#include <Arduino.h>
#include <FastLED.h>
#include <Servo.h>

#define STRIP1_PIN 36
#define STRIP2_PIN 37

#define NUM_LEDS1 120
#define NUM_LEDS2 120

CRGB t1[NUM_LEDS1];
CRGB t2[NUM_LEDS2];

int N1 = NUM_LEDS1;
int N2 = NUM_LEDS2;

Servo s1;
Servo s2;

int l1 = 32;
int l2 = 33;

extern bool gestoSiguiente;
extern bool gestoAnterior;
extern bool cambioAutomatico;

void EfectosEspeciales_begin() {

  FastLED.addLeds<WS2812B, STRIP1_PIN, GRB>(t1, N1);
  FastLED.addLeds<WS2812B, STRIP2_PIN, GRB>(t2, N2);
  FastLED.setBrightness(150);

  for (int i = 0; i < N1; i++) t1[i] = CRGB::Black;
  for (int i = 0; i < N2; i++) t2[i] = CRGB::Black;
  FastLED.show();

  pinMode(l1, OUTPUT);
  pinMode(l2, OUTPUT);
  digitalWrite(l1, LOW);
  digitalWrite(l2, LOW);
}

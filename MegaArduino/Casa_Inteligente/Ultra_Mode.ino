#include <Arduino.h>
#include <FastLED.h>
#include <Servo.h>

extern int modoActual;

extern CRGB t1[];
extern CRGB t2[];
extern int N1;
extern int N2;

extern Servo s1;
extern Servo s2;

extern int l1;
extern int l2;

#define NORMAL 0
#define ULTRA  2

unsigned long lastServoMove = 0;

int servoPos1 = 90;
int servoPos2 = 90;

int dir1 = 1;
int dir2 = -1;

void moverServos() {

    if (!s1.attached()) {
        s1.attach(34);
        s1.write(servoPos1);
    }

    if (!s2.attached()) {
        s2.attach(35);
        s2.write(servoPos2);
    }

    int velocidad = 16;

    servoPos1 += velocidad * dir1;
    if (servoPos1 >= 180) { servoPos1 = 180; dir1 = -1; }
    if (servoPos1 <= 0)   { servoPos1 = 0;   dir1 = 1; }

    servoPos2 += velocidad * dir2;
    if (servoPos2 >= 180) { servoPos2 = 180; dir2 = -1; }
    if (servoPos2 <= 0)   { servoPos2 = 0;   dir2 = 1; }

    s1.write(servoPos1);
    s2.write(servoPos2);
}

void rayosAmarillos() {

    for (int i = 0; i < N2; i++)
        t2[i] = CRGB::Black;

    int r = random(10, 20);

    for (int j = 0; j < r; j++) {
        int p = random(0, N2);
        t2[p] = CRGB(random(220,255), random(160,255), 0);
    }
}

void modoUltra() {

    if (modoActual != ULTRA) return;

    if (millis() - lastServoMove > 10) {
        lastServoMove = millis();
        moverServos();
    }

    for (int i = 0; i < N1; i++)
        t1[i] = CRGB::Black;

    rayosAmarillos();
    FastLED.show();

    digitalWrite(l1, HIGH);
    digitalWrite(l2, HIGH);
}

void apagarUltra() {

    digitalWrite(l1, LOW);
    digitalWrite(l2, LOW);

    for (int i = 0; i < N1; i++) t1[i] = CRGB::Black;
    for (int i = 0; i < N2; i++) t2[i] = CRGB::Black;
    FastLED.show();

    servoPos1 = 90;
    servoPos2 = 90;

    if (!s1.attached()) s1.attach(34);
    if (!s2.attached()) s2.attach(35);

    s1.write(90);
    s2.write(90);

    delay(70);

    s1.detach();
    s2.detach();
}

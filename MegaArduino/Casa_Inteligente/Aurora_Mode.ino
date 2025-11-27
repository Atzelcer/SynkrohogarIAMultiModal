#include <Arduino.h>
#include <FastLED.h>

extern int modoActual;
extern bool gestoSiguiente;
extern bool gestoAnterior;
extern bool cambioAutomatico;

extern CRGB t1[];
extern CRGB t2[];
extern int N1;
extern int N2;

#define NORMAL 0
#define AURORA 3

void aurora_rainbow() {
    static uint8_t h;
    fill_rainbow(t1, N1, h, 4);
    fill_rainbow(t2, N2, h + 80, 6);
    h++;
}

void aurora_smooth() {
    static uint8_t h;
    for (int i = 0; i < N1; i++) t1[i] = CHSV(h + i * 2, 255, 210);
    for (int i = 0; i < N2; i++) t2[i] = CHSV(h + i * 4, 255, 160);
    h++;
}

void aurora_stars() {
    for (int i = 0; i < N1; i++) t1[i] = CHSV(160, 80, random(120, 255));
    for (int i = 0; i < N2; i++) t2[i] = CHSV(90, 100, random(80, 200));
    t1[random(0, N1)] = CRGB::White;
    t2[random(0, N2)] = CRGB::Blue;
}

void aurora_twinkle() {
    for (int i = 0; i < N1; i++) t1[i] = CRGB::Black;
    for (int i = 0; i < N2; i++) t2[i] = CRGB::Black;
    t1[random(0, N1)] = CHSV(random(0, 255), 255, 255);
    t2[random(0, N2)] = CHSV(random(0, 255), 180, 200);
}

void aurora_wave() {
    static uint8_t h;
    for (int i = 0; i < N1; i++) t1[i] = CHSV(h + sin8(i * 3), 255, 200);
    for (int i = 0; i < N2; i++) t2[i] = CHSV(h + sin8(i * 6), 255, 150);
    h++;
}

void aurora_glow() {
    static uint8_t h;
    for (int i = 0; i < N1; i++) t1[i] = CHSV(h, 255, 180);
    for (int i = 0; i < N2; i++) t2[i] = CHSV(h + 90, 255, 120);
    h += 2;
}

void aurora_dual_color() {
    for (int i = 0; i < N1; i++) t1[i] = CHSV(140 + i, 255, 200);
    for (int i = 0; i < N2; i++) t2[i] = CHSV(200 + i * 2, 255, 160);
}

void aurora_pulse() {
    static uint8_t v = 0;
    for (int i = 0; i < N1; i++) t1[i] = CHSV(160, 255, v);
    for (int i = 0; i < N2; i++) t2[i] = CHSV(40, 255, v / 2);
    v = (v > 250) ? 0 : v + 5;
}

void aurora_shimmer() {
    for (int i = 0; i < N1; i++) t1[i] = CHSV(150, 200, random(80, 255));
    for (int i = 0; i < N2; i++) t2[i] = CHSV(30, 200, random(60, 180));
}

void aurora_comet() {
    static int p1 = 0;
    static int p2 = 0;
    for (int i = 0; i < N1; i++) t1[i] = CRGB::Black;
    for (int i = 0; i < N2; i++) t2[i] = CRGB::Black;
    t1[p1] = CHSV(160, 255, 255);
    t2[p2] = CHSV(10, 255, 255);
    p1 = (p1 + 1) % N1;
    p2 = (p2 + 2) % N2;
}

void aurora_stripes() {
    static uint8_t h;
    for (int i = 0; i < N1; i++) t1[i] = CHSV(h + (i % 10) * 10, 255, 180);
    for (int i = 0; i < N2; i++) t2[i] = CHSV(h + (i % 6) * 20, 255, 120);
    h++;
}

void aurora_sparkles() {
    static uint8_t h;
    for (int i = 0; i < N1; i++) t1[i] = CHSV(h, 200, 150);
    for (int i = 0; i < N2; i++) t2[i] = CHSV(h + 100, 200, 120);
    t1[random(0, N1)] = CRGB::White;
    t2[random(0, N2)] = CRGB::Aqua;
    h++;
}

void aurora_soft_rainbow() {
    static uint8_t h;
    fill_gradient_RGB(t1, N1, CHSV(h, 200, 180), CHSV(h + 60, 200, 180));
    fill_gradient_RGB(t2, N2, CHSV(h + 120, 200, 120), CHSV(h + 180, 200, 120));
    h++;
}

void aurora_mist() {
    static uint8_t h;
    for (int i = 0; i < N1; i++) t1[i] = CHSV(h + i, 60, 180);
    for (int i = 0; i < N2; i++) t2[i] = CHSV(h + i * 2, 40, 120);
    h++;
}

void aurora_horizon() {
    static uint8_t h;
    for (int i = 0; i < N1; i++) t1[i] = CHSV(100 + (i / 2), 255, 120 + (i % 40));
    for (int i = 0; i < N2; i++) t2[i] = CHSV(10 + (i / 3), 255, 80 + (i % 50));
    h++;
}

void aurora_cielo_nocturno() {
    for (int i = 0; i < N1; i++) {
        t1[i] = CRGB::Black;
        if (random8() < 10) t1[i] = CRGB(150, 150, 255);
    }
    for (int i = 0; i < N2; i++) {
        t2[i] = CRGB::Black;
        if (random8() < 14) t2[i] = CRGB(120, 120, 200);
    }
}

void aurora_tormenta() {
    fill_solid(t1, N1, CRGB::Black);
    fill_solid(t2, N2, CRGB::Black);
    for (int i = 0; i < 8; i++) {
        t1[random16(N1)] = CHSV(random8(), 255, 255);
        t2[random16(N2)] = CHSV(random8(), 255, 200);
    }
}

void aurora_rayos_azules() {
    static uint8_t t;
    for (int i = 0; i < N1; i++) t1[i] = CHSV(160, 255, sin8(i * 10 + t));
    for (int i = 0; i < N2; i++) t2[i] = CHSV(170, 255, sin8(i * 8 + t));
    t += 4;
}

typedef void (*efectoA)();
efectoA efectos[18] = {
    aurora_rainbow, aurora_smooth, aurora_stars, aurora_twinkle,
    aurora_wave, aurora_glow, aurora_dual_color, aurora_pulse,
    aurora_shimmer, aurora_comet, aurora_stripes, aurora_sparkles,
    aurora_soft_rainbow, aurora_mist, aurora_horizon,
    aurora_cielo_nocturno, aurora_tormenta, aurora_rayos_azules
};

int efectoActual = 0;
unsigned long ultimoCambio = 0;

void modoAurora() {

    if (modoActual != AURORA) return;

    efectos[efectoActual]();
    FastLED.show();

    if (cambioAutomatico) {
        if (millis() - ultimoCambio >= 8000) {
            efectoActual = (efectoActual + 1) % 18;
            ultimoCambio = millis();
        }
    }

    if (gestoSiguiente) {
        efectoActual = (efectoActual + 1) % 18;
        gestoSiguiente = false;
    }

    if (gestoAnterior) {
        efectoActual = (efectoActual == 0 ? 17 : efectoActual - 1);
        gestoAnterior = false;
    }
}

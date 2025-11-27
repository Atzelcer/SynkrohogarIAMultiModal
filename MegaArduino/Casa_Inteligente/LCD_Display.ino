#include <Wire.h>
#include <LiquidCrystal_I2C.h>

extern bool tecladoEnUso;
extern bool sistemaActivo;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void LCD_begin() {
    lcd.init();
    lcd.backlight();
    mostrarEnLCD("CASA SynkroHogar", "Iniciando...");
}

void mostrarEnLCD(const String &line1, const String &line2) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);

    if (line2.length() > 0) {
        lcd.setCursor(0, 1);
        lcd.print(line2);
    }
}

void mostrarPanelPrincipal() {

    String estado = sistemaActivo ?
        "Modo Super (SEGURO)" :
        "Modo Normal (OFF)";

    mostrarEnLCD("<< SynkroHogar >>", estado);
}

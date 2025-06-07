#include <display.h>
#include "Arduino.h"
#define debug
#include <debug.h>

#ifdef LCD

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const uint8_t COLUMS = 20;
const uint8_t ROWS = 4;
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

HIDisplay::HIDisplay()
{
}

void HIDisplay::init()
{
    initDebug();
    dbgOutLn("Welcome");
    while (lcd.begin(COLUMS, ROWS, LCD_5x8DOTS) != 1) // colums, rows, characters size
    {
        Serial.println(F("PCF8574 not connected or lcd pins is wrong."));
        delay(5000);
    }
    lcd.clear();
}

void HIDisplay::clear()
{
    lcd.clear();
}

void HIDisplay::setCursor(uint8_t column, uint8_t row)
{
    lcd.setCursor(column, row);
}

void HIDisplay::noBlink()
{
    lcd.noBlink();
};

void HIDisplay::blink()
{
    lcd.blink();
};

void HIDisplay::noCursor()
{
    lcd.noCursor();
};

void HIDisplay::cursor()
{
    lcd.cursor();
};

size_t HIDisplay::write(uint8_t character)
{
    return lcd.write(character);
}

#endif

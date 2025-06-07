#include <display.h>
#include "Arduino.h"
#define debug
#include <debug.h>

#ifdef OLED
#include <U8g2lib.h>

#define CHAR_WIDTH 1
#define CHAR_HEIGHT 1

U8X8_SH1106_128X64_NONAME_HW_I2C oled(19,18);

HIDisplay::HIDisplay()
{
}

void HIDisplay::init()
{
    initDebug();
    dbgOutLn("init display");
    oled.begin();
    oled.setFont(u8x8_font_5x7_r);
    oled.clearDisplay();
    oled.setPowerSave(1);
    delay(1000);
    oled.setPowerSave(0);
    
    oled.clear();
}

void HIDisplay::clear()
{
    oled.clear();
}

void HIDisplay::setCursor(uint8_t column, uint8_t row)
{
    oled.setCursor(column * CHAR_WIDTH, row * CHAR_HEIGHT);
}

void HIDisplay::noBlink() {
    // lcd.noBlink();
};

void HIDisplay::blink() {
    // lcd.blink();
};

void HIDisplay::noCursor() {
    // lcd.noCursor();
};

void HIDisplay::cursor() {
    // lcd.cursor();
};

size_t HIDisplay::write(uint8_t character)
{
    return oled.write(character);
}

#endif
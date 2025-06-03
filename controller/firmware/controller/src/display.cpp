#include <display.h>
#include "Arduino.h"
#include <TM1637Display.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "game.h"
#include "communication.h"


#define debug
#include <debug.h>

// this is the main switch between OLED and LC-Display
// The OLED is an 128x64 1" Pixeldisplay
// the LC-Display is an 20x4 LCD and en extra TM1637 Display for the time

// Switching to oled version
// #define OLED

#ifndef OLED

// forward declarations
void clearRow(uint8_t row);

char buffer[30];

// LC-Display definitions
const uint8_t COLUMS = 20;
const uint8_t ROWS = 4;
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

// 7-Segment LED Display
#define CLK 3
#define DIO 2
const uint8_t TTD[] = {SEG_F | SEG_G | SEG_E | SEG_D};
const uint8_t MND[] = {SEG_G};
TM1637Display seg7 = TM1637Display(CLK, DIO);

LCDisplay::LCDisplay()
{
}

void LCDisplay::init(byte brightness)
{
    dbgOutLn(F("display init"));
    seg7.clear();
    setBrightness(brightness);
    seg7.setSegments(TTD, 1, 0);

    while (lcd.begin(COLUMS, ROWS, LCD_5x8DOTS) != 1) // colums, rows, characters size
    {
        dbgOutLn(F("PCF8574 not connected or lcd pins is wrong."));
        delay(5000);
    }

    lcd.clear();
};

void LCDisplay::setBrightness(byte brightness)
{
    m_brightness = brightness;
    seg7.setBrightness(m_brightness > 1);
};

void LCDisplay::setDifficulty(byte difficulty)
{
    m_difficulty = difficulty;
};

void LCDisplay::showTime(long act) {
};

void LCDisplay::clearTime()
{
    seg7.clear();
};

void LCDisplay::clear()
{
    lcd.clear();
};

void LCDisplay::printHeader(bool withVersion)
{
  lcd.setCursor(0, 0);
  lcd.print(F("HI-Tane "));
  if (withVersion)
    lcd.print(F(Version));

  strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[m_difficulty])));
  lcd.print(buffer[0]);
}

void LCDisplay::printWelcome()
{
  lcd.setCursor(0, 1);
  lcd.print(F("Welcome"));
}

void LCDisplay::showError(byte error) {
      clearRow(2);
      strcpy_P(buffer, (char *)pgm_read_word(&(ERROR_MESSAGES[error])));
      lcd.print(buffer);
}

void clearRow(uint8_t row)
{
  lcd.setCursor(0, row);
  lcd.print("                    ");
  lcd.setCursor(0, row);
}


#endif
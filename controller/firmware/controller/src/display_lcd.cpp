#include <display.h>

#ifdef LCD

#include "Arduino.h"
#define debug
#include <debug.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <globals.h>
#include <game.h>

const uint8_t COLUMS = 20;
const uint8_t ROWS = 4;
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

HIDisplay::HIDisplay() {}

void HIDisplay::init(HTCOM &htcom, Indicators &indicators, SerialNumber &serialnumber)
{
    m_htcom = &htcom;
    m_indicators = &indicators;
    m_serialnumber = &serialnumber;
    initDebug();
    dbgOutLn("Welcome");
    while (lcd.begin(COLUMS, ROWS, LCD_5x8DOTS) != 1) // colums, rows, characters size
    {
        Serial.println(F("PCF8574 not connected or lcd pins is wrong."));
        delay(5000);
    }
    lcd.clear();
}

void HIDisplay::setBrightness(uint8_t brightness)
{
    m_brightness = brightness;
}

void HIDisplay::clear()
{
    lcd.clear();
}

void HIDisplay::noBlink()
{
    lcd.noBlink();
};

void HIDisplay::blink()
{
    lcd.blink();
};

size_t HIDisplay::write(uint8_t character)
{
    return lcd.write(character);
}

// convenient functions for the game controller
void HIDisplay::printHeader(bool withVersion)
{
    clearrow(0);
    char buffer[30];
    lcd.setCursor(0, 0);
    lcd.print(F(lb_game_name));
    if (withVersion)
        lcd.print(F(lb_version));

    lcd.print(" ");
    strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[m_htcom->getDifficulty()])));
    lcd.print(buffer[0]);
}

void HIDisplay::printStatus()
{
    char buffer[30];
    clearrow(3);
    m_serialnumber->String(buffer);
    lcd.print(buffer);

    // print indicators
    for (byte x = 0; x < m_indicators->Count(); x++)
    {
        byte idx = m_indicators->Get(x);
        lcd.print(" ");
        if (m_indicators->IsActive(x))
        {
            lcd.print(char(0x23));
        }
        else
        {
        }
        strcpy_P(buffer, (char *)pgm_read_word(&(INDICATORNAMES[idx])));
        lcd.print(buffer);
    }
};

void HIDisplay::printWelcome()
{
    clearrow(1);
    lcd.setCursor(0, 1);
    lcd.print(F(lb_welcome));
}

void HIDisplay::showTime(int act)
{
    lcd.setCursor(15, 0);
    bool neg = act < 0;
    int t = abs(act);
    byte sec = t % 60;
    byte min = (t - sec) / 60;
    if (neg)
    {
        lcd.print("-");
        if (min <= 0)
            lcd.print("0");
        lcd.print(min);
    }
    else
    {
        if (min <= 9)
            lcd.print("0");
        lcd.print(min);
    }
    lcd.print(":");
    if (sec <= 9)
        lcd.print("0");
    lcd.print(sec);
}

void HIDisplay::showStrikes()
{
    byte strikes = m_htcom->getStrikes();
    for (uint8_t x = 0; x < strikes; x++)
    {
        lcd.setCursor(11 + x, 0);
        lcd.print('*');
    }
}

void HIDisplay::printError(byte err)
{
    char buffer[30];
    clearrow(2);
    strcpy_P(buffer, (char *)pgm_read_word(&(ERROR_MESSAGES[err])));
    lcd.print(buffer);
}

void HIDisplay::clearError()
{
    clearrow(2);
}

void HIDisplay::resolved()
{
    lcd.setCursor(0, 1);
    lcd.print(F(lb_resolved_1));
    lcd.setCursor(0, 2);
    lcd.print(F(lb_resolved_2));
    lcd.setCursor(0, 3);
    lcd.print(F(lb_resolved_3));
};

void HIDisplay::fullyStriked()
{
    lcd.setCursor(0, 1);
    lcd.print(F(lb_striked_1));
    lcd.setCursor(0, 2);
    lcd.print(F(lb_striked_2));
    lcd.setCursor(0, 3);
    lcd.print(F(lb_resolved_3));
};

void HIDisplay::clearrow(uint8_t row)
{
    lcd.setCursor(0, row);
    lcd.print(F("                    "));
    lcd.setCursor(0, row);
}

void HIDisplay::showMenuKey(byte mk)
{
    char buffer[30];
    clearrow(1);
    strcpy_P(buffer, (char *)pgm_read_word(&(MENU_KEYS[mk])));
    lcd.print(buffer);
}

void HIDisplay::setCursorToValue()
{
    clearrow(2);
    lcd.setCursor(0, 2);
}

void HIDisplay::showValueCursor()
{
    lcd.setCursor(0, 2);
    lcd.cursor();
};

void HIDisplay::hideValueCursor()
{
    lcd.noCursor();
    lcd.noBlink();
};

void HIDisplay::hideMenu() {
    clearrow(1);
    clearrow(2);
    hideValueCursor();
}

void HIDisplay::printModules()
{
    char buffer[30];
    uint8_t mcnt = m_htcom->installedModuleCount();

    clearrow(1);
    clearrow(2);
    dbgOut(F("print modules "));
    dbgOutLn2(mcnt, DEC);
    lcd.setCursor(0, 1);
    for (byte idx = 0; idx < mcnt; idx++)
    {
        byte mod = m_htcom->getInstalledModuleID(idx);
        if (mod != ID_NONE)
        {
            strcpy_P(buffer, (char *)pgm_read_word(&(MODULE_LABELS[mod - MOD_OFFSET])));
            lcd.print(buffer);
            if (m_htcom->isModuleState(mod, ModuleState::DISARMED))
            {
                lcd.print(F(" "));
            }
            else
            {
                lcd.print(F("*"));
            }
        }
        if (idx >= 5)
        {
            lcd.setCursor(0, 2);
        }
    }
}

#endif

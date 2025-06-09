#include <display.h>

#ifdef OLED

#include "Arduino.h"
#define debug
#include <debug.h>
#include <U8g2lib.h>
#include <globals.h>
#include <game.h>

U8X8_SH1106_128X64_NONAME_HW_I2C oled(19, 18);

HIDisplay::HIDisplay()
{
}

void HIDisplay::init(HTCOM &htcom, Indicators &indicators, SerialNumber &serialnumber)
{
    m_htcom = &htcom;
    m_indicators = &indicators;
    m_serialnumber = &serialnumber;
    initDebug();
    dbgOutLn("init display");
    oled.begin();
    oled.setFont(u8x8_font_8x13_1x2_f);
    oled.clearDisplay();
    oled.setPowerSave(1);
    delay(1000);
    oled.setPowerSave(0);

    oled.clear();
}

void HIDisplay::setBrightness(uint8_t brightness)
{
    m_brightness = brightness;
}

void HIDisplay::clear()
{
    oled.clear();
}

void HIDisplay::noBlink() {
    // lcd.noBlink();
};

void HIDisplay::blink() {
    // lcd.blink();
};

size_t HIDisplay::write(uint8_t character)
{
    return oled.write(character);
}

// convenient functions for the game controller
void HIDisplay::printHeader(bool withVersion)
{
    clearrow(0);
    char buffer[30];
    oled.setCursor(0, 0);
    oled.print(F("HI-Tane "));
    if (withVersion)
        oled.print(F(Version));

    oled.print(" ");
    strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[m_htcom->getDifficulty()])));
    oled.print(buffer[0]);
}

void HIDisplay::printStatus() {
    /*
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
    */
};

void HIDisplay::printWelcome()
{
    clearrow(2);
    oled.setCursor(0, 2);
    oled.print(F("Welcome"));
}

void HIDisplay::showTime(int act)
{
    /*
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
    */
}

void HIDisplay::showStrikes()
{
    /*
    byte strikes = m_htcom->getStrikes();
    for (uint8_t x = 0; x < strikes; x++)
    {
        lcd.setCursor(11 + x, 0);
        lcd.print('*');
    }
    */
}

void HIDisplay::printError(byte err)
{
    /*
    char buffer[30];
    clearrow(2);
    strcpy_P(buffer, (char *)pgm_read_word(&(ERROR_MESSAGES[err])));
    lcd.print(buffer);
    */
}

void HIDisplay::clearError()
{
    clearrow(2);
}

void HIDisplay::resolved() {
    /*
    lcd.setCursor(0, 1);
    lcd.print(F("Hurray, you "));
    lcd.setCursor(0, 2);
    lcd.print(F("defused the bomb"));
    lcd.setCursor(0, 3);
    lcd.print(F("Another game? <yes>"));
    */
};

void HIDisplay::fullyStriked() {
    /*
    lcd.setCursor(0, 1);
    lcd.print(F("sorry, you exploded!"));
    lcd.setCursor(0, 3);
    lcd.print(F("Another game? <yes>"));
    */
};

void HIDisplay::clearrow(uint8_t row)
{
    oled.setCursor(0, row);
    oled.print("                ");
    oled.setCursor(0, row);
}

void HIDisplay::showMenuKey(byte mk)
{
    char buffer[30];
    clearrow(2);
    strcpy_P(buffer, (char *)pgm_read_word(&(MENU_KEYS[mk])));
    oled.print(buffer);
}

void HIDisplay::setCursorToValue()
{
    clearrow(4);
    oled.setCursor(1, 4);
}

void HIDisplay::showValueCursor()
{
    oled.setCursor(0, 4);
    oled.print("*");
};

void HIDisplay::hideValueCursor() {
    oled.setCursor(0, 4);
    oled.print(" ");
};

void HIDisplay::hideMenu()
{
    clearrow(2);
    clearrow(4);
    hideValueCursor();
}

void HIDisplay::printModules()
{
    /*
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
    */
}

#endif
#include <display.h>

#ifdef OLED

#include "Arduino.h"
#define debug
#include <debug.h>
#include <U8g2lib.h>
#include <globals.h>
#include <game.h>

U8X8_SH1106_128X64_NONAME_HW_I2C oled(19, 18);

HIDisplay::HIDisplay() {}

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

void HIDisplay::noBlink() {};

void HIDisplay::blink() {};

size_t HIDisplay::write(uint8_t character)
{
    return oled.write(character);
}

// convenient functions for the game controller
void HIDisplay::printHeader(bool withVersion)
{
    char buffer[30];
    clearrow(0);
    oled.print(F(lb_game_name));
    oled.print(" ");
    if (withVersion)
    {
        oled.print(F(lb_version));
        oled.print(" ");
    }

    strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[m_htcom->getDifficulty()])));
    oled.print(buffer[0]);
}

void HIDisplay::printStatus()
{
    char buffer[30];
    clearrow(4);
    m_serialnumber->String(buffer);
    oled.print(buffer);
    clearrow(6);
    // print indicators
    for (byte x = 0; x < m_indicators->Count(); x++)
    {
        byte idx = m_indicators->Get(x);
        if (x > 0)
        {
            oled.print(" ");
        }
        if (m_indicators->IsActive(x))
        {
            oled.print(char(0x23));
        }
        strcpy_P(buffer, (char *)pgm_read_word(&(INDICATORNAMES[idx])));
        oled.print(buffer);
    }
};

void HIDisplay::hideStatus()
{
    clearrow(4);
    clearrow(6);
}

void HIDisplay::printWelcome()
{
    clearrow(2);
    oled.print(F(lb_welcome));
}

void HIDisplay::showTime(int act)
{
    oled.setCursor(11, 0);
    bool neg = act < 0;
    int t = abs(act);
    byte sec = t % 60;
    byte min = (t - sec) / 60;
    if (neg)
    {
        oled.print("-");
        if (min <= 0)
            oled.print("0");
        oled.print(min);
    }
    else
    {
        if (min <= 9)
            oled.print("0");
        oled.print(min);
    }
    oled.print(":");
    if (sec <= 9)
        oled.print("0");
    oled.print(sec);
}

void HIDisplay::showStrikes()
{
    if (m_htcom->hasNewStrikes()) {
        clearrow(2);
        byte strikes = m_htcom->getStrikes();
        for (uint8_t x = 0; x < strikes; x++)
        {
            oled.print('*');
        }
    }
}

void HIDisplay::printError(byte err)
{
    char buffer[30];
    clearrow(2);
    strcpy_P(buffer, (char *)pgm_read_word(&(ERROR_MESSAGES[err])));
    oled.print(buffer);
}

void HIDisplay::clearError()
{
    clearrow(2);
}

void HIDisplay::resolved()
{
    oled.clearDisplay();
    oled.setCursor(0, 2);
    oled.print(F(lb_resolved_1));
    oled.setCursor(0, 4);
    oled.print(F(lb_resolved_2));
    oled.setCursor(0, 6);
    oled.print(F(lb_resolved_3));
};

void HIDisplay::fullyStriked()
{
    oled.clearDisplay();
    oled.setCursor(0, 3);
    oled.print(F(lb_striked_1));
    oled.setCursor(0, 6);
    oled.print(F(lb_resolved_3));
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

void HIDisplay::hideValueCursor()
{
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
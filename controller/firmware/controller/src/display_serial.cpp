#include <display.h>

#ifdef NN

#include "Arduino.h"
#define debug
#include <debug.h>
#include <globals.h>
#include <game.h>
#include <retroTerm.h>

retroTerm terminal;

HIDisplay::HIDisplay()
{
}

void HIDisplay::init(HTCOM &htcom, Indicators &indicators, SerialNumber &serialnumber)
{
    m_htcom = &htcom;
    m_indicators = &indicators;
    m_serialnumber = &serialnumber;
    Serial.begin(115200);
    terminal.begin(Serial);
    terminal.reset();
    terminal.setTitle(lb_game_name);
    terminal.hideCursor();
}

void HIDisplay::setBrightness(uint8_t brightness)
{
    m_brightness = brightness;
}

void HIDisplay::clear()
{
    terminal.eraseScreen();
}

void HIDisplay::noBlink()
{
    terminal.hideCursor();
};

void HIDisplay::blink()
{
    terminal.showCursor();
};

size_t HIDisplay::write(uint8_t character)
{
    terminal.print(character);
    return 1;
}

// convenient functions for the game controller
void HIDisplay::printHeader(bool withVersion)
{
    char buffer[30];
    clearrow(0);
    terminal.print(F(lb_game_name));
    terminal.print(" ");
    if (withVersion)
    {
        terminal.print(F(lb_version));
        terminal.print(" ");
    }

    strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[m_htcom->getDifficulty()])));
    terminal.println(buffer[0]);
}

void HIDisplay::printStatus()
{
    char buffer[30];
    clearrow(2);
    m_serialnumber->String(buffer);
    terminal.print(buffer);
    // print indicators
    for (byte x = 0; x < m_indicators->Count(); x++)
    {
        byte idx = m_indicators->Get(x);
        terminal.print(" ");
        if (m_indicators->IsActive(x))
        {
            terminal.print(char(0x23));
        }
        strcpy_P(buffer, (char *)pgm_read_word(&(INDICATORNAMES[idx])));
        terminal.print(buffer);
    }
    terminal.println();
};

void HIDisplay::hideStatus()
{
    clearrow(2);
}

void HIDisplay::printWelcome()
{
    clearrow(1);
    terminal.println(F(lb_welcome));
}

void HIDisplay::showTime(int act)
{
    terminal.println(F("<setcursor 11, 0>"));
    bool neg = act < 0;
    int t = abs(act);
    byte sec = t % 60;
    byte min = (t - sec) / 60;
    if (neg)
    {
        terminal.print("-");
        if (min <= 0)
            terminal.print("0");
        terminal.print(min);
    }
    else
    {
        if (min <= 9)
            terminal.print("0");
        terminal.print(min);
    }
    terminal.print(":");
    if (sec <= 9)
        terminal.print("0");
    terminal.print(sec);
    terminal.println();
}

void HIDisplay::showStrikes()
{
    clearrow(2);
    terminal.print(F("strikes: "));
    byte strikes = m_htcom->getStrikes();
    for (uint8_t x = 0; x < strikes; x++)
    {
        terminal.print('*');
    }
    terminal.println();
}

void HIDisplay::printError(byte err)
{
    char buffer[30];
    clearrow(2);
    terminal.print(F("error: "));
    strcpy_P(buffer, (char *)pgm_read_word(&(ERROR_MESSAGES[err])));
    terminal.println(buffer);
}

void HIDisplay::clearError()
{
    clearrow(2);
}

void HIDisplay::resolved()
{
    clear();
    terminal.print(F(lb_resolved_1));
    terminal.print(' ');
    terminal.println(F(lb_resolved_2));
    newGame();
};

void HIDisplay::fullyStriked()
{
    clear();
    terminal.println(F(lb_striked_1));
    newGame();
};

void HIDisplay::newGame()
{
    terminal.println(F(lb_new_game));
}

void HIDisplay::clearrow(uint8_t row)
{
    terminal.moveCursorTo(0, row);
    terminal.deleteLine();
    terminal.moveCursorTo(0, row);
}

void HIDisplay::showMenuKey(byte mk)
{
    char buffer[30];
    clearrow(1);
    strcpy_P(buffer, (char *)pgm_read_word(&(MENU_KEYS[mk])));
    terminal.println(buffer);
}

void HIDisplay::setCursorToValue()
{
    clearrow(2);
    terminal.moveCursorTo(1, 2);
}

void HIDisplay::showValueCursor()
{
    terminal.moveCursorTo(0, 2);
    terminal.print("*");
};

void HIDisplay::hideValueCursor()
{
    terminal.moveCursorTo(0, 2);
    terminal.print(" ");
};

void HIDisplay::hideMenu()
{
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
    terminal.moveCursorTo(0, 1);
    for (byte idx = 0; idx < mcnt; idx++)
    {
        byte mod = m_htcom->getInstalledModuleID(idx);
        if (mod != ID_NONE)
        {
            strcpy_P(buffer, (char *)pgm_read_word(&(MODULE_LABELS[mod - MOD_OFFSET])));
            terminal.print(buffer);
            if (m_htcom->isModuleState(mod, ModuleState::DISARMED))
            {
                terminal.print(F(" "));
            }
            else
            {
                terminal.print(F("*"));
            }
        }
        if (idx >= 5)
        {
            terminal.moveCursorTo(0, 2);
        }
    }
}

#endif

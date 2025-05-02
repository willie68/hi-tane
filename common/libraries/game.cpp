#include "game.h"
#include <Adafruit_NeoPixel.h>
// #define debug
#include <debug.h>
#include <serialnumber.h>
#include <indicators.h>

void nextDiff(Difficulty &diff)
{
    int idx = static_cast<int>(diff);
    idx++;
    if (idx == NUM_DIFF)
    {
        idx--;
    }
    diff = static_cast<Difficulty>(idx);
}

void prevDiff(Difficulty &diff)
{
    int idx = static_cast<int>(diff);
    idx--;
    if (idx < 0)
    {
        idx = 0;
    }
    diff = static_cast<Difficulty>(idx);
}

#ifdef debug
char bf1[30];
#endif

Game::Game(ModuleTag moduleTag, byte pinStatusLED)
{
    m_state = ModuleState::INIT;
    m_tag = moduleTag;
    m_statusLED = pinStatusLED;
    m_difficulty = Difficulty::SIMPLE;
};

Game::Game()
{
    m_state = ModuleState::INIT;
    m_tag = ModuleTag::WIRES;
    m_statusLED = 4;
    m_difficulty = Difficulty::SIMPLE;
};

void Game::init()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(m_statusLED, OUTPUT);
    m_pixel = new Adafruit_NeoPixel(1, m_statusLED, NEO_RGB + NEO_KHZ800);
    m_pixel->setPixelColor(0, PX_BLACK);
    m_pixel->setBrightness(127);
    m_pixel->show();

    m_htcom = new HTCOM();
    m_htcom->attach(m_tag);

    setState(ModuleState::INIT);
};

void Game::setTestParameter()
{
    SerialNumber serialNumber;
    serialNumber.Generate();

    m_indicators.Add(INDICATOR::CLR, false);
    m_indicators.Add(INDICATOR::IND, true);
    m_indicators.Add(INDICATOR::NSA, false);
}

bool Game::hasIndicator(INDICATOR ind, bool active)
{
    word inds = m_htcom->getIndicators();
    m_indicators.Decompress(inds);
    return m_indicators.hasIndicator(ind, active);
};

Indicators Game::getIndicators()
{
    return m_indicators;
}

bool Game::isSerialnumberOdd()
{
    return false;
};

word Game::getGameTime()
{
    return m_htcom->getGameTime();
};

void Game::arm()
{
    dbgOutLn(F("ARMED"));
    setState(ModuleState::ARMED);
}

void Game::setStrike()
{
    dbgOutLn(F("STRIKE"));
    setState(ModuleState::STRIKED);
};

void Game::setSolved()
{
    dbgOutLn(F("SOLVED"));
    setState(ModuleState::DISARMED);
};

Difficulty Game::getGameDifficulty()
{
    return (Difficulty)m_htcom->getDifficulty();
}

uint32_t Game::getSerialNumber()
{
    return m_htcom->getSerialNumber();
}

bool Game::snrHasVocal()
{
    uint32_t sn = m_htcom->getSerialNumber();
    SerialNumber serialNumber;
    serialNumber.Set(sn);
#ifdef debug
    dbgOut(F("snr:"));
    dbgOut2(sn, HEX);
    dbgOut(F(" "));
    serialNumber.String(bf1);
    dbgOutLn(bf1)
#endif

        return serialNumber.isVocal();
}

bool Game::snrIsLastDigitOdd()
{
    uint32_t sn = m_htcom->getSerialNumber();
    SerialNumber serialNumber;
    serialNumber.Set(sn);
#ifdef debug
    dbgOut(F("snr:"));
    dbgOut2(sn, HEX);
    dbgOut(F(" "));
    serialNumber.String(bf1);
    dbgOutLn(bf1);
#endif
    return serialNumber.isLastDigitOdd();
}

bool Game::isGameDifficulty(Difficulty difficulty)
{
    return m_htcom->getDifficulty() == byte(difficulty);
};

void Game::setState(ModuleState state)
{
#ifdef debug
    if (m_state != state)
    {
        Serial.print(F("Module state:"));
        Serial.println(state);
    }
#endif
    m_state = state;
    switch (m_state)
    {
    case UNKNOWN:
    case INIT:
        setPixelColor(PX_BLUE);
        break;
    case ARMED:
        m_htcom->sendArmed();
        setPixelColor(PX_RED);
        break;
    case STRIKED:
        m_htcom->sendStrike();
        setPixelColor(PX_RED);
        m_strikeTime = millis() + 3000;
        break;
    case DISARMED:
        m_htcom->sendDisarmed();
        setPixelColor(PX_GREEN);
        break;
    }
};

bool Game::isState(ModuleState state)
{
    return m_state == state;
};

void Game::sendError(byte err)
{
    m_htcom->sendError(err);
}

void Game::setPixelColor(uint32_t color)
{
    m_pixel->setPixelColor(0, color);
    m_pixel->setBrightness(m_htcom->getBrightness() * 16);
    m_pixel->show();
}

void Game::poll()
{
    m_htcom->poll();
    if (m_state == ModuleState::STRIKED)
    {
        unsigned long ms = millis();
        if (((ms % 250) < 125) && (ms < m_strikeTime))
        {
            setPixelColor(PX_RED);
        }
        else
        {
            setPixelColor(PX_BLACK);
        }
        if (ms > m_strikeTime)
        {
            setPixelColor(PX_RED);
            m_state = ModuleState::ARMED;
        }
    }
    if (m_htcom->isNewAmbSettings())
    {
        byte brightness = m_htcom->getBrightness() * 16;
        if (brightness != m_pixel->getBrightness())
        {
            m_pixel->setBrightness(brightness);
            m_pixel->show();
        }
    }
}

void Game::showTime(bool fast)
{
    word act = m_htcom->getGameTime();
    if (act != m_saveTime)
    {
        m_saveTime = act;
        if (fast)
        {
            Serial.println(act, DEC);
            return;
        }
        bool neg = act < 0;
        int t = abs(act);
        byte sec = t % 60;
        byte min = (t - sec) / 60;
        if (neg)
        {
            Serial.print("-");
            if (min <= 0)
                Serial.print("0");
            Serial.print(min);
        }
        else
        {
            if (min <= 9)
                Serial.print("0");
            Serial.print(min);
        }
        Serial.print(":");
        if (sec <= 9)
            Serial.print("0");
        Serial.println(sec);
    }
}

bool Game::hasNewStrikes()
{
    return m_htcom->hasNewStrikes();
}

byte Game::getStrikes()
{
    return m_htcom->getStrikes();
};

bool Game::isNewGameSettings()
{
    return m_htcom->isNewGameSettings();
}

void Game::setGameDifficulty(Difficulty difficulty)
{
    m_htcom->setGameDifficulty(difficulty);
}

byte Game::getBrightness()
{
    return m_htcom->getBrightness();
}

void Game::sendBeep()
{
    dbgOutLn(F("m: beep"));
    m_htcom->sendBeep();
}

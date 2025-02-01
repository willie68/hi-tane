#include "game.h"
#include <Adafruit_NeoPixel.h>
#include <debug.h>

void nextDiff(Difficulty &diff)
{
    int idx = static_cast<int>(diff);
    idx++;
    if (idx == HARD) {
        idx--;
    }
    diff = static_cast<Difficulty>(idx);
}

void prevDiff(Difficulty &diff)
{
    int idx = static_cast<int>(diff);
    idx--;
    if (idx <0) {
        idx = 0;
    }
    diff = static_cast<Difficulty>(idx);
}

Game::Game(ModuleTag moduleTag, byte pinStatusLED, byte pinCom)
{
    tag = moduleTag;
    StatusLED = pinStatusLED;
    Com = pinCom;
};

Game::Game()
{
    state = ModuleState::INIT;
    tag = ModuleTag::WIRES;
    StatusLED = 4;
    Com = 11;
    difficulty = Difficulty::HARD;
};

void Game::init()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(StatusLED, OUTPUT);
    pixel = new Adafruit_NeoPixel(1, StatusLED, NEO_RGB + NEO_KHZ800);
    pixel->setPixelColor(0, PX_BLACK);
    pixel->setBrightness(127);
    pixel->show();

    htcom = new HTCOM();
    htcom->attach(Com, tag);

    setState(ModuleState::INIT);
};

bool Game::hasIndicator(INDICATOR ind)
{
    return false;
};
bool Game::isSerialnumberOdd()
{
    return false;
};
word Game::getGameTime()
{
    return 3600;
};

void Game::arm() {
    dbgOutLn(F("ARMED"));
    setState(ModuleState::ARMED);
}

void Game::setStrike() {
    dbgOutLn(F("STRIKE"));
    setState(ModuleState::STRIKED);
};

void Game::setSolved() {
    dbgOutLn(F("SOLVED"));
    setState(ModuleState::DISARMED);
};

void Game::setGameDifficulty(Difficulty difficulty)
{
    this->difficulty = difficulty;
}

Difficulty Game::getGameDifficulty()
{
    return difficulty;
}

bool Game::isGameDifficulty(Difficulty difficulty)
{
    return this->difficulty == difficulty;
};

void Game::setState(ModuleState state)
{
#ifdef debug
    if (this->state != moduleState)
    {
        Serial.print(F("Module state:"));
        Serial.println(state);
    }
#endif
    this->state = state;
    switch (state)
    {
    case INIT:
        setPixelColor(PX_BLUE);
        break;
    case ARMED:
        setPixelColor(PX_RED);
        break;
    case STRIKED:
        htcom->sendStrike();
        setPixelColor(PX_RED);
        break;
    case DISARMED:
        htcom->sendDisarmed();
        setPixelColor(PX_GREEN);
        break;
    }
};

bool Game::isState(ModuleState state)
{
    return this->state == state;
};

void Game::sendError(byte err)
{
    htcom->sendError(err);
}

void Game::setPixelColor(uint32_t color)
{
    pixel->setPixelColor(0, color);
    pixel->show();
}

void Game::poll()
{
    htcom->poll();
    if (state == ModuleState::STRIKED)
    {
        if ((millis() % 250) < 125)
        {
            pixel->setPixelColor(0, PX_RED);
        }
        else
        {
            pixel->setPixelColor(0, PX_BLACK);
        }
        pixel->show();
    }
}

void Game::showTime()
{
    word act = getGameTime();
    if (act != saveTime)
    {
        saveTime = act;
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
            if (min <= 0)
                Serial.print("0");
            Serial.print(min);
        }
        Serial.print(":");
        if (sec <= 0)
            Serial.print("0");
        Serial.println(sec);
    }
}

void Game::setIntLED(bool on) {
    digitalWrite(LED_BUILTIN, on);
}
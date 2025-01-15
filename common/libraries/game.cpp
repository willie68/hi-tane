#include "game.h"
#include <Adafruit_NeoPixel.h>

Game::Game(ModuleTag moduleTag, byte pinStatusLED, byte pinCom)
{
    tag = moduleTag;
    LED = pinStatusLED;
    Com = pinCom;
};

Game::Game()
{
    state = ModuleState::INIT;
    tag = ModuleTag::WIRES;
    LED = 4;
    Com = 11;
};

void Game::init()
{
    pinMode(LED, OUTPUT);
    pixel = Adafruit_NeoPixel(1, LED, NEO_GRB + NEO_KHZ800);
    pixel.setPixelColor(0, PX_GREEN);
    pixel.show();

    htcom = HTCOM();
    htcom.attach(Com, tag);

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
    Serial.println("ARMED");
    setState(ModuleState::ARMED);
}

void Game::setStrike() {
    Serial.println("STRIKE");
    setState(ModuleState::STRIKED);
};

void Game::setSolved() {
    Serial.println("SOLVED");
    setState(ModuleState::DISARMED);
};

bool Game::isGameDifficulty(Difficulty difficulty)
{
    return difficulty == Difficulty::SIMPLE;
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
        htcom.sendStrike();
        setPixelColor(PX_RED);
        break;
    case DISARMED:
        htcom.sendDisarmed();
        setPixelColor(PX_GREEN);
        break;
    }
};

bool Game::isState(ModuleState state)
{
    return this->state == state;
};

void Game::sendError(const void *msg)
{
    htcom.sendError(msg);
}

void Game::setPixelColor(uint32_t color)
{
    pixel.setPixelColor(0, color);
    pixel.show();
}

void Game::poll()
{
    htcom.poll();
    if (state == ModuleState::STRIKED)
    {
        if ((millis() % 250) < 125)
        {
            pixel.setPixelColor(0, PX_RED);
        }
        else
        {
            pixel.setPixelColor(0, PX_BLACK);
        }
        pixel.show();
    }
}

void Game::showTime()
{
  word  act = getGameTime();
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
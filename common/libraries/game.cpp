#include "game.h"
#include <Adafruit_NeoPixel.h>

Game::Game()
{
    state = ModuleState::INIT;
};

void Game::init(ModuleTag moduleTag, byte pinStatusLED, byte pinCom)
{
    Serial.print("status pin ");
    Serial.println(pinStatusLED);

    pinMode(pinStatusLED, OUTPUT);
    pixel = Adafruit_NeoPixel(1, pinStatusLED, NEO_GRB + NEO_KHZ800);
    pixel.setPixelColor(0, PX_GREEN);
    pixel.show();

    htcom = HTCOM();
    htcom.attach(pinCom, moduleTag);

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

void Game::setStrike() {
};

void Game::setSolved() {
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

void Game::sendError(const void *msg) {
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
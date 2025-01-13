#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <indicators.h>
#include <Adafruit_NeoPixel.h>
#include <communication.h>

using serial_t = char[6];

const uint32_t PX_BLACK = Adafruit_NeoPixel::Color(0, 0, 0);
const uint32_t PX_RED = Adafruit_NeoPixel::Color(0xff, 0, 0);
const uint32_t PX_BLUE = Adafruit_NeoPixel::Color(0, 0, 0xff);
const uint32_t PX_GREEN = Adafruit_NeoPixel::Color(0, 0xff, 0);
const uint32_t PX_YELLOW = Adafruit_NeoPixel::Color(0xff, 0xff, 0);
const uint32_t PX_WHITE = Adafruit_NeoPixel::Color(0xff, 0xff, 0xff);
const uint32_t PX_BROWN = Adafruit_NeoPixel::Color(0x5b, 0x3a, 0x29);

enum ModuleState
{
    INIT,
    ARMED,
    STRIKED,
    DISARMED
};

enum ModuleTag
{
    CONTROLLER = 44,
    WIRES = 45,
    MAZE = 46
};

enum Difficulty
{
    SIMPLE = 1,
    MEDIUM,
    HARD
};

class Game
{
public:
    Game();

    void init(ModuleTag moduleTag, byte pinStatusLED, byte pinCom);
    bool hasIndicator(INDICATOR ind);
    bool isSerialnumberOdd();
    word getGameTime();
    void setStrike();
    void setSolved();
    bool isGameDifficulty(Difficulty difficulty);
    void setState(ModuleState state);
    bool isState(ModuleState state);

    void sendError(const void *msg);

    void setPixelColor(uint32_t color);

    void poll();
private:
    Adafruit_NeoPixel pixel;
    ModuleState state;
    HTCOM htcom;
};
#endif
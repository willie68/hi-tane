// game 
// this class abstracts the hitane game from the odule site.
// it will manage the game state, status led, module tag
// the module itself can get information about the game itself,
// like parts about the difficulty, serialnumber, game time, 
// central game strikes, communication with the controller...

#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <indicators.h>
#include <Adafruit_NeoPixel.h>
#include <communication.h>

using serial_t = char[6];

const uint32_t PX_BLACK = Adafruit_NeoPixel::Color(0, 0, 0);
const uint32_t PX_RED = Adafruit_NeoPixel::Color(0xff, 0, 0);
const uint32_t PX_GREEN = Adafruit_NeoPixel::Color(0, 0xff, 0);
const uint32_t PX_BLUE = Adafruit_NeoPixel::Color(0, 0, 0xff);
const uint32_t PX_YELLOW = Adafruit_NeoPixel::Color(0xff, 0xff, 0);
const uint32_t PX_WHITE = Adafruit_NeoPixel::Color(0xff, 0xff, 0xff);
const uint32_t PX_LOWWHITE = Adafruit_NeoPixel::Color(0x20, 0x20, 0x20);
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
    MAZE = 46,
    PASSWORD = 47,
    MORSE = 48
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
    Game(ModuleTag moduleTag, byte pinStatusLED, byte pinCom);
    Game();

    void init();
    bool hasIndicator(INDICATOR ind);
    bool isSerialnumberOdd();
    word getGameTime();
    void arm();
    void setStrike();
    void setSolved();

    Difficulty getGameDifficulty();
    bool isGameDifficulty(Difficulty difficulty);
    void setState(ModuleState state);
    bool isState(ModuleState state);

    void sendError(const void *msg);

    void setPixelColor(uint32_t color);

    void poll();

    // output game time for debug to serial
    void showTime();
    void setGameDifficulty(Difficulty difficulty);
private:
    ModuleTag tag;
    byte StatusLED;
    byte Com;
    word saveTime;
    Adafruit_NeoPixel *pixel;
    ModuleState state;
    HTCOM *htcom;
    Difficulty difficulty;
};
#endif
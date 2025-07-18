// game
// this class abstracts the hitane game from the module site.
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
#include <difficulty.h>
#include <globals.h>

using serial_t = char[6];

const uint32_t PX_BLACK = Adafruit_NeoPixel::Color(0, 0, 0);
const uint32_t PX_RED = Adafruit_NeoPixel::Color(0xff, 0, 0);
const uint32_t PX_GREEN = Adafruit_NeoPixel::Color(0, 0xff, 0);
const uint32_t PX_BLUE = Adafruit_NeoPixel::Color(0, 0, 0xff);
const uint32_t PX_YELLOW = Adafruit_NeoPixel::Color(0xff, 0xff, 0);
const uint32_t PX_WHITE = Adafruit_NeoPixel::Color(0xff, 0xff, 0xff);
const uint32_t PX_LOWWHITE = Adafruit_NeoPixel::Color(0x20, 0x20, 0x20);
const uint32_t PX_BROWN = Adafruit_NeoPixel::Color(0x5b, 0x3a, 0x29);

enum ModuleTag
{
    CONTROLLER = ID_CONTROLLER,
    WIRES = ID_WIRES,
    MAZE = ID_MAZE,
    PASSWORD = ID_PASSWORD,
    MORSE = ID_MORSE,
    SIMON = ID_SIMON,
    NEEDY_GAS = ID_NGAS,
    SYMBOLS = ID_SYMBOLS,
    NEEDY_CAPACITY = ID_NCAPACITY
};


enum ERRORS
{
    ERR_NO_ERR = 0,
    ERR_INVALID_WIRES = 1,
    ERR_INVALID_CONFIGURATION = 2
};

class Game
{
public:
    Game(ModuleTag moduleTag, byte pinStatusLED);
    Game();

    void init();

    bool hasIndicator(INDICATOR ind);
    Indicators getIndicators();

    word getGameTime();
    void arm();
    void setStrike();
    void setSolved();

    bool hasNewStrikes();
    byte getStrikes();
    byte getBrightness();

    uint32_t getSerialNumber();
    bool snrHasVocal();
    bool snrIsLastDigitOdd();

    Difficulty getGameDifficulty();
    bool isGameDifficulty(Difficulty difficulty);
    void setState(ModuleState state);
    bool isState(ModuleState state);
    bool isPaused();

    void sendError(byte err);

    void setPixelColor(uint32_t color);

    void poll();

    // output game time for debug to serial
    void showTime(bool fast);

    bool hasIndicator(INDICATOR indicator, bool active);
    bool isNewGameSettings();

    void setGameDifficulty(Difficulty difficulty);

    void setTestParameter();

    void sendBeep();

private:
    HTCOM *m_htcom;

    ModuleTag m_tag;
    byte m_statusLED;
    word m_saveTime;
    Adafruit_NeoPixel *m_pixel;
    ModuleState m_state;
    Indicators m_indicators;
    byte m_difficulty;
    unsigned long m_strikeTime;
};
#endif
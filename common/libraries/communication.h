
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>

const byte DEFAULT_BRIGHTNESS = 4;
const byte CS_PIN = 10;

enum MODULEIDS
{
    ID_CONTROLLER = 44,
    ID_WIRES = 45,
    ID_MAZE = 46,
    ID_SIMON = 47,
    ID_PASSWORD = 48,
    ID_MORSE = 49,
    ID_MAX_MODULES = 50
};
const byte MODULE_COUNT = 6;
const byte modules[MODULE_COUNT] = {ID_CONTROLLER, ID_WIRES, ID_MAZE, ID_SIMON, ID_PASSWORD, ID_MORSE};

const char e_nn[] PROGMEM = "";
const char e_we[] PROGMEM = "wires: invalid wire";

const char *const ERROR_MESSAGES[] PROGMEM = {
    e_nn, e_we};

enum COMMANDS
{
    MID_HEARTBEAT = 0x0001,
    MID_AMBIENTSETTINGS = 0x0002,
    MID_GAMESETTINGS = 0x0003,
    MID_ERROR = 0x0300,
    MID_DISARM = 0x0400,
    MID_STRIKE = 0x0500
};

enum PARAMETER
{
    PAR_DISARM = 1,
    PAR_ARM,
};

class HTCOM
{
public:
    HTCOM(byte id);
    HTCOM();

    void attach(byte id);
    void poll();

    // module functions
    void sendError(byte err);
    void sendDisarmed();
    void sendStrike();
    int getGameTime();
    byte getDifficulty();

    bool hasNewStrikes();
    byte getStrikes();
    byte getBrightness();
    bool isNewAmbSettings();
    bool isNewGameSettings();

    void setGameDifficulty(byte dif);

    // Controller only functions
    void setCtlrStrikes(byte strikes);
    void setCtrlSerialNumber(uint32_t srn);
    void setCtrlDifficulty(byte difficulty);
    void setCtrlBrightness(byte brightness);
    void setCtrlIndicators(word inds);

    void sendCtrlHearbeat(word countdown);
    void sendGameSettings();
    void sendAmbientSettings();

    bool hasCtrlError();
    byte getCtrlError();
    void setCtrlError(byte error);
    void resetError();
protected:
    byte moduleID;

    int gametime;
    word inds;

    bool newStrike;
    byte strikes;
    byte difficulty;

    bool newAmbSettings; // new ambient settings received
    bool newGameSettings; // new game settings received
    byte brightness;

    bool hasError;
    byte lastError;
    unsigned long errTime;

    uint32_t snr;

    can_frame sndCanMsg;
    can_frame rcvCanMsg;
    MCP2515 *mcp2515;
};

#endif
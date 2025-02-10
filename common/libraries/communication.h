
#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#define COM 1

#include <Arduino.h>
#include <PJONSoftwareBitBang.h>

const byte DEFAULT_BRIGHTNESS = 4;

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

const char e_nn[] PROGMEM = "";
const char e_we[] PROGMEM = "wires: invalid wire";

const char *const ERROR_MESSAGES[] PROGMEM = {
    e_nn, e_we};

enum COMMANDS
{
    CMD_HEARTBEAT = 1,
    CMD_STRIKE,
    CMD_GAMESETTINGS,
    CMD_ERROR,
    CMD_ARM,
    CMD_AMBIENTSETTINGS
};

enum PARAMETER
{
    PAR_DISARM = 1,
    PAR_ARM,
};

class HTCOM
{
public:
    HTCOM(byte pin, byte id);
    HTCOM();

    void attach(byte pin, byte id);
    void poll();

    // module functions
    void sendError(byte err);
    void sendDisarmed();
    void sendStrike();
    int getGameTime();

    bool hasNewStrikes();
    byte getStrikes();
    byte getBrightness();
    bool isNewAmbSettings();

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

    void busReceive();

    // internal use only
    void receive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &info);

protected:
    PJONSoftwareBitBang bus;
    byte moduleID;

    int gametime;
    word inds;

    bool newStrike;
    byte strikes;
    byte difficulty;

    bool newAmbSettings; // new ambient settings received
    byte brightness;

    bool hasError;
    byte lastError;
    unsigned long errTime;

    uint32_t snr;
    byte sndbuf[8];
    byte recbuf[8];

    void sendAll(const void *buf, byte size, bool once);
};

#endif

#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#define COM 1

#include <Arduino.h>
#include <PJONSoftwareBitBang.h>

#define ID_CONTROLLER 44
#define ID_WIRES 45
#define ID_MAZE 46

const char e_nn[] PROGMEM = "";
const char e_we[] PROGMEM = "wires: invalid wire";

const char *const ERROR_MESSAGES[] PROGMEM = {
    e_nn, e_we};

enum COMMANDS
{
    CMD_HEARTBEAT = 1,
    CMD_STRIKE,
    CMD_GAMESETTINGS,
    CMD_AMBIENTSETTINGS
};

class HTCOM
{
public:
    HTCOM(uint8_t pin, uint8_t id);
    HTCOM();

    void attach(uint8_t pin, uint8_t id);
    void poll();

    // module functions
    void sendError(const void *msg);
    void sendDisarmed();
    void sendStrike();
    int getGameTime();
    byte getStrikes();

    // Controller only functions
    void setCtlrStrikes(bool strikes[]);
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

    // internal use only
    void receive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &info);

protected:
    PJONSoftwareBitBang bus;
    int gametime;
    word inds;
    byte strikes;
    byte difficulty;
    byte brightness;
    bool hasError;
    byte lastError;
    uint32_t snr;

    void sendAll(const void *buf);
};

#endif
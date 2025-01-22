
#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#define COM 1

#include <Arduino.h>
#include <PJONSoftwareBitBang.h>

#define ID_CONTROLLER 44
#define ID_WIRES 45
#define ID_MAZE 46

using serial_t = char[6];

enum COMMANDS
{
    CMD_HEARTBEAT = 1,
    CMD_STRIKE,
    CMD_GAMESETTINGS
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
    void sendHearbeat(word countdown, word flags);
    void setCtlrStrikes(byte strikes);
    void setCtrlSerialNumber(serial_t serialnumber);
    void setCtrlDifficulty(byte difficulty);

    // internal use only
    void receive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &info);

protected:
    PJONSoftwareBitBang bus;
    serial_t serialnumber;
    int gametime;
    word flags;
    byte strikes;
    byte difficulty;

    void sendAll(const void *buf);
};

#endif
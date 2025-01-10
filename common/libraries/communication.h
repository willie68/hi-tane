
#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#define COM 1

#include <Arduino.h>
#include <PJONSoftwareBitBang.h>

#define ID_CONTROLLER 44
#define ID_WIRES 45

using serial_t = char[6];

enum COMMANDS
{
    CMD_HEARTBEAT = 1,
    CMD_STRIKE
};

class HTCOM
{
public:
    HTCOM(uint8_t pin, uint8_t id);
    HTCOM();

    void attach(uint8_t pin, uint8_t id);
    void poll();

    void set_SerialNumber(serial_t serialnumber);

    void sendHearbeat(word countdown, word flags);

    void sendError(const void *msg);
    void sendDisarmed();
    void sendStrike();

    int getGameTime();

    byte getStrikes();

    // Controller only functions
    void setStrikes(byte strikes);

    // internal use only
    void receive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &info);

protected:
    PJONSoftwareBitBang bus;
    serial_t serialnumber;
    int gametime;
    word flags;
    byte strikes;
};

#endif
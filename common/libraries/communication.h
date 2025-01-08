
#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#define COM 1

#include <Arduino.h>
#include <PJONSoftwareBitBang.h>

#define ID_CONTROLLER 44
#define ID_WIRES      45

using serial_t = char[6];

class HTCOM
{
public:
    HTCOM(uint8_t pin, uint8_t id);
    HTCOM();

    void attach(uint8_t pin, uint8_t id);
    void poll();

    void set_SerialNumber(serial_t serialnumber);

    void sendError(const void *msg);
    void sendDisarmed();
    void sendStrike();

protected:
    PJONSoftwareBitBang bus;
    serial_t serialnumber;
};

#endif
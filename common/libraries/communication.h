#define COM 1
#include <PJONSoftwareBitBang.h>

#ifndef communication_h
#define communication_h

using serial_t = char[6];

class HTCOM
{
public:
    HTCOM(uint8_t pin, uint8_t id);
    HTCOM();

    void attach(uint8_t pin, uint8_t id);
    void poll();

    void set_SerialNumber(serial_t serialnumber);

protected:
    PJONSoftwareBitBang bus;
    serial_t serialnumber;
};

#endif
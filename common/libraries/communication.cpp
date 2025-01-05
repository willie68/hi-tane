#include "communication.h"
#include <PJONSoftwareBitBang.h>

HTCOM::HTCOM()
{
    this->bus = PJONSoftwareBitBang();
}

void HTCOM::attach(uint8_t pin, uint8_t id)
{
    this->bus.set_id(id);
    this->bus.strategy.set_pin(pin);
    this->bus.begin();
    this->bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device 44 every second
}

void HTCOM::set_SerialNumber(serial_t serialnumber)
{
    strncpy(this->serialnumber, serialnumber, 6);
    this->bus.send(PJON_BROADCAST, this->serialnumber, 6);
}

void HTCOM::poll()
{
    this->bus.update();
};
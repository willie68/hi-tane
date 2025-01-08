#include "communication.h"
#include <PJONSoftwareBitBang.h>

HTCOM::HTCOM()
{
    bus = PJONSoftwareBitBang();
}

HTCOM::HTCOM(uint8_t pin, uint8_t id)
{
    bus = PJONSoftwareBitBang();
    attach(pin, id);
}

void HTCOM::attach(uint8_t pin, uint8_t id)
{
    bus.set_id(id);
    bus.strategy.set_pin(pin);
    bus.begin();
}

void HTCOM::set_SerialNumber(serial_t srn)
{
    strncpy(serialnumber, srn, 6);
    bus.send(PJON_BROADCAST, serialnumber, 6);
}

void HTCOM::poll()
{
    bus.update();
};

void HTCOM::sendError(const void *msg) {
    bus.send(ID_CONTROLLER, msg, 40);
}

void HTCOM::sendDisarmed() {
    bus.send(ID_CONTROLLER, "wires: disarmed", 16);
}

void HTCOM::sendStrike() {
    bus.send(ID_CONTROLLER, "wires: strike", 14);
}
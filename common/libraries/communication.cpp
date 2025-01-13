#include "communication.h"
#include <PJONSoftwareBitBang.h>

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &info)
{
    if (info.custom_pointer)
        ((HTCOM *)info.custom_pointer)->receive(payload, length, info);
};

void HTCOM::receive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &info)
{
    byte cmd = payload[0];
    switch (cmd)
    {
    case CMD_HEARTBEAT:
        gametime = (payload[1] << 8) + payload[2];
        flags = (payload[3] << 8) + payload[4];
        strikes = payload[5];
        break;
    default:
        break;
    };
};

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
    gametime = 3600;
    bus.set_id(id);
    bus.strategy.set_pin(pin);
    bus.set_receiver(receiver_function);
    bus.set_custom_pointer(this);
    bus.begin();
}

void HTCOM::setCtrlSerialNumber(serial_t srn)
{
    strncpy(serialnumber, srn, 6);
    bus.send(PJON_BROADCAST, serialnumber, 6);
}

void HTCOM::poll()
{
    bus.receive(100);
    bus.update();
};

void HTCOM::sendHearbeat(word countdown, word flags)
{
    byte buf[6];
    buf[0] = CMD_HEARTBEAT;
    buf[1] = countdown >> 8;
    buf[2] = countdown & 0x00FF;
    buf[3] = flags >> 8;
    buf[4] = flags & 0x00FF;
    buf[5] = strikes;

    sendAll(&buf);
}

void HTCOM::sendAll(const void *buf)
{
    bus.send(ID_WIRES, buf, 6);
    bus.send(ID_MAZE, buf, 6);
}

void HTCOM::sendError(const void *msg)
{
    bus.send(ID_CONTROLLER, msg, 40);
}

void HTCOM::sendDisarmed()
{
    bus.send(ID_CONTROLLER, "wires: disarmed", 16);
}

void HTCOM::sendStrike()
{
    bus.send(ID_CONTROLLER, "wires: strike", 14);
}


void HTCOM::setCtlrStrikes(byte strikes)
{
    this->strikes = strikes;
}

void HTCOM::setCtrlDifficulty(byte difficulty)
{
    this->difficulty = difficulty;

    byte buf[6];
    buf[0] = CMD_GAMESETTINGS;
    buf[1] = this->difficulty;
    buf[2] = 0;
    buf[3] = 0;
    buf[4] = 0;
    buf[5] = 0;

    sendAll(&buf);
}

byte HTCOM::getStrikes()
{
    return this->strikes;
}

int HTCOM::getGameTime()
{
    return gametime;
}

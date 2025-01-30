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
        strikes = payload[3];
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

void HTCOM::setCtrlSerialNumber(uint32_t srn)
{
    this->snr = snr;
}

void HTCOM::poll()
{
    bus.receive(100);
    bus.update();
};

void HTCOM::sendCtrlHearbeat(word countdown)
{
    byte buf[6];
    buf[0] = CMD_HEARTBEAT;
    buf[1] = countdown >> 8;
    buf[2] = countdown & 0x00FF;
    buf[3] = strikes;
    buf[4] = 0;
    buf[5] = 0;

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

void HTCOM::setCtrlIndicators(word inds)
{
    this->inds = inds;
}

void HTCOM::setCtlrStrikes(bool strikes[])
{
    this->strikes = 0;
    this->strikes = this->strikes | strikes[0] | strikes[1] << 1 | strikes[2] << 2;
}

void HTCOM::setCtrlDifficulty(byte difficulty)
{
    this->difficulty = difficulty;
}

void HTCOM::setCtrlBrightness(byte brightness)
{
    this->brightness = brightness;
    sendAmbientSettings();
}

byte HTCOM::getStrikes()
{
    return this->strikes;
}

int HTCOM::getGameTime()
{
    return gametime;
}

void HTCOM::sendGameSettings()
{
    byte buf[8];
    buf[0] = CMD_GAMESETTINGS;
    buf[1] = this->difficulty;
    buf[2] = inds >> 8;
    buf[3] = inds & 0x00FF;
    buf[4] = snr & 0xff;
    buf[5] = (snr >> 8) & 0xff;
    buf[6] = (snr >> 16) & 0xff;
    buf[7] = 0;
    sendAll(&buf);
}

void HTCOM::sendAmbientSettings()
{
    byte buf[6];
    buf[0] = CMD_AMBIENTSETTINGS;
    buf[1] = this->brightness;
    buf[2] = 0;
    buf[3] = 0;
    buf[4] = 0;
    buf[5] = 0;

    sendAll(&buf);
}

void HTCOM::setCtrlError(byte error)
{
    lastError = error;
}

bool HTCOM::hasCtrlError()
{
    return hasError;
}
byte HTCOM::getCtrlError()
{
    return lastError;
}

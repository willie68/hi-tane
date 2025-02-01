#include "communication.h"
#include <PJONSoftwareBitBang.h>
#define debug
#include <debug.h>

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &info)
{
    if (info.custom_pointer)
        ((HTCOM *)info.custom_pointer)->receive(payload, length, info);
};

void HTCOM::receive(uint8_t *payload, uint16_t length, const PJON_Packet_Info &info)
{
    dbgOutLn("ht: rec");
    switch (payload[0])
    {
    case CMD_HEARTBEAT:
        gametime = (payload[1] << 8) + payload[2];
        strikes = payload[3];
        break;
    case CMD_ERROR:
        setCtrlError(payload[2]);
        break;
    case CMD_AMBIENTSETTINGS:
        brightness = (payload[1]);
        break;
    case CMD_GAMESETTINGS:
        difficulty = payload[1];
        inds = payload[2] +payload[3] << 8 ;
        snr = payload[4] + (paylod[5] << 8) + (paylod[6] << 16)
        break;
    default:
        dbgOut("unkown: ");
        dbgOutLn2(payload[0], HEX) break;
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
    moduleID = id;
}

void HTCOM::attach(uint8_t pin, uint8_t id)
{
    gametime = 3600;
    bus.set_id(id);
    bus.strategy.set_pin(pin);
    bus.set_receiver(receiver_function);
    bus.set_custom_pointer(this);
    bus.begin();
    moduleID = id;
    resetError();
}

void HTCOM::setCtrlSerialNumber(uint32_t srn)
{
    this->snr = snr;
}

void HTCOM::poll()
{
    bus.receive(100);
    bus.update();
    if (hasError && (millis() > errTime))
        resetError();
};

void HTCOM::sendCtrlHearbeat(word countdown)
{
    sndbuf[0] = CMD_HEARTBEAT;
    sndbuf[1] = countdown >> 8;
    sndbuf[2] = countdown & 0x00FF;
    sndbuf[3] = strikes;

    sendAll(&sndbuf, 4, false);
}

void HTCOM::sendAll(const void *buf, byte size, bool once = false)
{
    byte mod = ID_CONTROLLER;
    while (mod < ID_MAX_MODULES)
    {
        if (moduleID != mod)
        {
            if (once)
            {
                bus.send_packet(mod, buf, size);
            }
            else
            {
                bus.send(mod, buf, size);
            }
        }
        mod++;
    }
}

void HTCOM::sendError(byte err)
{
    sndbuf[0] = CMD_ERROR;
    sndbuf[1] = moduleID;
    sndbuf[2] = err;
    sndbuf[3] = 0;
    sndbuf[4] = 0;
    sndbuf[5] = 0;
    sndbuf[6] = 0;
    sndbuf[7] = 0;

    bus.send(ID_CONTROLLER, sndbuf, 8);
}

void HTCOM::sendDisarmed()
{
    sndbuf[0] = CMD_ARM;
    sndbuf[1] = moduleID;
    sndbuf[2] = PAR_DISARM;
    sndbuf[3] = 0;
    sndbuf[4] = 0;
    sndbuf[5] = 0;
    sndbuf[6] = 0;
    sndbuf[7] = 0;

    bus.send(ID_CONTROLLER, sndbuf, 8);
}

void HTCOM::sendStrike()
{
    sndbuf[0] = CMD_STRIKE;
    sndbuf[1] = moduleID;

    bus.send(ID_CONTROLLER, sndbuf, 2);
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

byte HTCOM::getBrightness()
{
    return this->brightness;
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
    sndbuf[0] = CMD_GAMESETTINGS;
    sndbuf[1] = this->difficulty;
    sndbuf[2] = inds & 0x00FF;
    sndbuf[3] = inds >> 8;
    sndbuf[4] = snr & 0xff;
    sndbuf[5] = (snr >> 8) & 0xff;
    sndbuf[6] = (snr >> 16) & 0xff;
    sendAll(&sndbuf, 7);
}

void HTCOM::sendAmbientSettings()
{
    sndbuf[0] = CMD_AMBIENTSETTINGS;
    sndbuf[1] = this->brightness;

    sendAll(&sndbuf, 2);
}

void HTCOM::setCtrlError(byte error)
{
    lastError = error;
    hasError = true;
    errTime = millis() + 5000;
}

void HTCOM::resetError()
{
    errTime = 0;
    hasError = false;
    lastError = 0;
}

bool HTCOM::hasCtrlError()
{
    return hasError;
}

byte HTCOM::getCtrlError()
{
    return lastError;
}

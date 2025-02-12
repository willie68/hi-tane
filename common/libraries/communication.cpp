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
    switch (payload[0])
    {
    case CMD_HEARTBEAT:
        dbgOutLn("hb");
        gametime = (payload[1] << 8) + payload[2];
        strikes = payload[3];
        break;
    case CMD_ERROR:
        setCtrlError(payload[2]);
        break;
    case CMD_AMBIENTSETTINGS:
        brightness = (payload[1]);
        newAmbSettings = true;
        break;
    case CMD_GAMESETTINGS:
        difficulty = payload[1];
        inds = payload[2] + (payload[3] << 8);
        snr = uint32_t(payload[4]) + (uint32_t(payload[5]) << 8) + (uint32_t(payload[6]) << 16);
        break;
    case CMD_STRIKE:
        dbgOutLn(F("s"));
        strikes++;
        newStrike = true;
        break;
    default:
        dbgOut("unkown: ");
        dbgOutLn2(payload[0], HEX);
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
    newAmbSettings = false;
    newStrike = false;
    strikes = 0;
    brightness = DEFAULT_BRIGHTNESS;
}

void HTCOM::withInterrupt(bool wi)
{
    wint = wi;
}

void HTCOM::busReceive()
{
    bus.receive();
}

void HTCOM::poll()
{
    if (!wint)
        bus.receive(100); // only do a receive, when no interrupt methode active

    bus.update();
    if (hasError && (millis() > errTime))
        resetError();
}

void HTCOM::setCtrlSerialNumber(uint32_t srn)
{
    this->snr = snr;
}

void HTCOM::sendCtrlHearbeat(word countdown)
{
    sndbuf[0] = CMD_HEARTBEAT;
    sndbuf[1] = countdown >> 8;
    sndbuf[2] = countdown & 0x00FF;
    sndbuf[3] = strikes;

    sendAll(&sndbuf, 4, true);
}

void HTCOM::sendAll(const void *buf, byte size, bool once = false)
{
    for (byte i = 0; i < MODULE_COUNT; i++)
    {
        byte mod = modules[i];
        if (moduleID != mod)
        {
            if (once)
            {
                bus.send_packet(mod, buf, size);
                return;
            }
            bus.send(mod, buf, size);
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

void HTCOM::setCtlrStrikes(byte strikes)
{
    this->strikes = strikes;
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

bool HTCOM::hasNewStrikes()
{
    if (newStrike)
    {
        newStrike = false;
        return true;
    }
    return true;
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

bool HTCOM::isNewAmbSettings()
{
    if (newAmbSettings)
    {
        newAmbSettings = false;
        return true;
    }
    return false;
}

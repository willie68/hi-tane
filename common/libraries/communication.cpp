#include "communication.h"
#include <SPI.h>
#include <mcp2515.h>
//#define debug
#include <debug.h>

HTCOM::HTCOM()
{
}

HTCOM::HTCOM(uint8_t id)
{
    moduleID = id;
}

void HTCOM::attach(uint8_t id)
{
    mcp2515 = new MCP2515(CS_PIN);
    gametime = 3600;
    moduleID = id;
    resetError();
    newAmbSettings = false;
    newGameSettings = false;
    newStrike = false;
    strikes = 0;
    brightness = DEFAULT_BRIGHTNESS;

    mcp2515->reset();
    mcp2515->setBitrate(CAN_500KBPS);
    mcp2515->setNormalMode();
}

void HTCOM::poll()
{
    if (mcp2515->readMessage(&rcvCanMsg) == MCP2515::ERROR_OK)
    {
        unsigned long canID = rcvCanMsg.can_id;
        dbgOut("msg from ");
        dbgOutLn2(canID, HEX);
        byte rcvModule = ID_CONTROLLER;
        if (canID > 0x0000ff)
        {
            rcvModule = canID & 0x0000ff;
            canID = canID & 0x00ff00;
        }
        if (moduleID == ID_CONTROLLER)
        {
            // the controller is only interested in non controller messages
            if (rcvModule == ID_CONTROLLER)
                return;
        }
        else
        {
            // the game Modules are only interested in controller messages
            if (rcvModule != ID_CONTROLLER)
                return;
        }
        switch (canID)
        {
        case MID_HEARTBEAT:
            dbgOutLn("hb");
            gametime = (rcvCanMsg.data[0] << 8) + rcvCanMsg.data[1];
            strikes = rcvCanMsg.data[2];
            break;
        case MID_AMBIENTSETTINGS:
            brightness = (rcvCanMsg.data[0]);
            newAmbSettings = true;
            break;
        case MID_GAMESETTINGS:
            dbgOut("gs ");
            difficulty = rcvCanMsg.data[0];
            dbgOut2(difficulty, HEX);
            inds = rcvCanMsg.data[1] + (rcvCanMsg.data[2] << 8);
            dbgOut(" ");
            dbgOut2(inds, HEX);
            snr = uint32_t(rcvCanMsg.data[3]) + (uint32_t(rcvCanMsg.data[4]) << 8) + (uint32_t(rcvCanMsg.data[5]) << 16);
            dbgOut(" ");
            dbgOutLn2(snr, HEX);
            newGameSettings = true;
            break;
        case MID_ERROR:
            setCtrlError(rcvCanMsg.data[0]);
            break;
        case MID_STRIKE:
            dbgOutLn(F("s"));
            strikes++;
            newStrike = true;
            break;
        default:
            dbgOut("unkown: ");
            dbgOut2(rcvModule, HEX);
            dbgOut(" ")
                dbgOutLn2(rcvCanMsg.can_id, HEX);
            break;
        };
    }
    // check if there is an message incoming
    if (hasError && (millis() > errTime))
        resetError();
}

void HTCOM::setCtrlSerialNumber(uint32_t srn)
{
    this->snr = snr;
}

void HTCOM::sendCtrlHearbeat(word countdown)
{
    sndCanMsg.can_id = MID_HEARTBEAT;
    sndCanMsg.can_dlc = 3;
    sndCanMsg.data[0] = countdown >> 8;
    sndCanMsg.data[1] = countdown & 0x00FF;
    sndCanMsg.data[2] = strikes;

    mcp2515->sendMessage(&sndCanMsg);
}

void HTCOM::sendError(byte err)
{
    sndCanMsg.can_id = MID_ERROR + moduleID;
    sndCanMsg.can_dlc = 1;
    sndCanMsg.data[0] = err;

    mcp2515->sendMessage(&sndCanMsg);
}

void HTCOM::sendDisarmed()
{
    sndCanMsg.can_id = MID_DISARM + moduleID;
    sndCanMsg.can_dlc = 1;
    sndCanMsg.data[0] = moduleID;

    mcp2515->sendMessage(&sndCanMsg);
}

void HTCOM::sendStrike()
{
    sndCanMsg.can_id = MID_STRIKE + moduleID;
    sndCanMsg.can_dlc = 1;
    sndCanMsg.data[0] = moduleID;

    mcp2515->sendMessage(&sndCanMsg);
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
    dbgOut("df: ");
    dbgOutLn2(difficulty, HEX);
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
    sndCanMsg.can_id = MID_GAMESETTINGS;
    sndCanMsg.can_dlc = 6;
    sndCanMsg.data[0] = this->difficulty;
    sndCanMsg.data[1] = inds & 0x00FF;
    sndCanMsg.data[2] = inds >> 8;
    sndCanMsg.data[3] = snr & 0xff;
    sndCanMsg.data[4] = (snr >> 8) & 0xff;
    sndCanMsg.data[5] = (snr >> 16) & 0xff;

    mcp2515->sendMessage(&sndCanMsg);
}

void HTCOM::sendAmbientSettings()
{
    sndCanMsg.can_id = MID_AMBIENTSETTINGS;
    sndCanMsg.can_dlc = 1;
    sndCanMsg.data[0] = this->brightness;

    mcp2515->sendMessage(&sndCanMsg);
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

bool HTCOM::isNewGameSettings()
{
    if (newGameSettings)
    {
        newGameSettings = false;
        return true;
    }
    return false;
}

byte HTCOM::getDifficulty()
{
    return difficulty;
}

void HTCOM::setGameDifficulty(byte dif) {
    difficulty = dif;
}
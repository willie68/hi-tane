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
#ifndef HI_MODULE
    resetCtrlError();
    initModules();
#endif
    newAmbSettings = false;
    newGameSettings = false;
    newStrike = false;
    strikes = 0;
    brightness = DEFAULT_BRIGHTNESS;

    mcp2515->reset();
    mcp2515->setBitrate(CAN_500KBPS);
    mcp2515->setNormalMode();
}

#ifndef HI_MODULE
void HTCOM::initModules()
{
    for (byte x = 0; x < MAX_INSTALLED_MODULES; x++)
    {
        installedModules[x] = 0;
        stateOfModules[x] = ModuleState::UNKNOWN;
    }
}
#endif

void HTCOM::poll()
{
    if (mcp2515->readMessage(&rcvCanMsg) == MCP2515::ERROR_OK)
    {
        unsigned long canID = rcvCanMsg.can_id;
        dbgOut(F("msg: 0x"));
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
        case MID_INITIALISATION:
            sendModuleID();
            break;
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
            dbgOut("gs:");
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
#ifndef HI_MODULE
        case MID_ERROR:
            setCtrlError(rcvCanMsg.data[0]);
            break;
        case MID_MODULEINIT:
            dbgOut(F("mint: "));
            dbgOutLn2(rcvModule, DEC);
            addToModuleList(rcvModule);
            break;
        case MID_STATE:
            dbgOut(F("mst: "));
            dbgOut2(rcvModule, DEC);
            dbgOut(F(":"));
            dbgOutLn2(rcvCanMsg.data[1], DEC);
            updateModule(rcvModule, static_cast<ModuleState>(rcvCanMsg.data[1]));
            break;
        case MID_BEEP:
            dbgOutLn(F("beep"));
            toBeep = true;
            break;
#endif
        case MID_STRIKE:
            dbgOutLn(F("s"));
            strikes++;
            newStrike = true;
            break;
        default:
            dbgOut(F("unk: "));
            dbgOut2(rcvModule, DEC);
            dbgOut(":");
            dbgOutLn2(rcvCanMsg.can_id, DEC);
            break;
        };
    }
#ifndef HI_MODULE
    // check if there is an message incoming
    if (hasError && (millis() > errTime))
        resetCtrlError();
#endif
}

uint32_t HTCOM::getSerialNumber()
{
    return snr;
}

word HTCOM::getIndicators()
{
    return inds;
}

void HTCOM::sendModuleID()
{
    byte dl = moduleID - ID_CONTROLLER;
    delay(dl* 100);
    sndCanMsg.can_id = MID_MODULEINIT + moduleID;
    sndCanMsg.can_dlc = 1;
    sndCanMsg.data[0] = moduleID;

    mcp2515->sendMessage(&sndCanMsg);
}

void HTCOM::sendError(byte err)
{
    sndCanMsg.can_id = MID_ERROR + moduleID;
    sndCanMsg.can_dlc = 1;
    sndCanMsg.data[0] = err;

    mcp2515->sendMessage(&sndCanMsg);
}

void HTCOM::sendArmed()
{
    sendModuleState(ModuleState::ARMED);
}

void HTCOM::sendDisarmed()
{
    sendModuleState(ModuleState::DISARMED);
}

void HTCOM::sendModuleState(ModuleState state)
{
    sndCanMsg.can_id = MID_STATE + moduleID;
    sndCanMsg.can_dlc = 2;
    sndCanMsg.data[0] = moduleID;
    sndCanMsg.data[1] = state;

    mcp2515->sendMessage(&sndCanMsg);
}

void HTCOM::sendStrike()
{
    sndCanMsg.can_id = MID_STRIKE + moduleID;
    sndCanMsg.can_dlc = 1;
    sndCanMsg.data[0] = moduleID;

    mcp2515->sendMessage(&sndCanMsg);
}

void HTCOM::sendBeep()
{
    sndCanMsg.can_id = MID_BEEP;
    sndCanMsg.can_dlc = 1;
    sndCanMsg.data[0] = moduleID;

    mcp2515->sendMessage(&sndCanMsg);
}

byte HTCOM::getBrightness()
{
    return this->brightness;
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

#ifndef HI_MODULE
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

void HTCOM::setCtrlBrightness(byte brightness)
{
    this->brightness = brightness;
    sendCtrlAmbientSettings();
}

// game settings will be sendet to all modules
void HTCOM::sendCtrlGameSettings()
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

// here all ambient settings will be sended to all modules
void HTCOM::sendCtrlAmbientSettings()
{
    sndCanMsg.can_id = MID_AMBIENTSETTINGS;
    sndCanMsg.can_dlc = 1;
    sndCanMsg.data[0] = this->brightness;

    mcp2515->sendMessage(&sndCanMsg);
}

// this will be sended to evaluate which modules are installed.
void HTCOM::sendCtrlInitialisation()
{
    sndCanMsg.can_id = MID_INITIALISATION;
    sndCanMsg.can_dlc = 1;
    sndCanMsg.data[0] = 0xff;

    mcp2515->sendMessage(&sndCanMsg);
}

void HTCOM::setCtrlError(byte error)
{
    lastError = error;
    hasError = true;
    errTime = millis() + 5000;
}

void HTCOM::resetCtrlError()
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

void HTCOM::addToModuleList(byte moduleID)
{
    for (byte x = 0; x < MAX_INSTALLED_MODULES; x++)
    {
        // module already installed ?
        if (installedModules[x] == moduleID) {
            break;
        }
        // free space 
        if (installedModules[x] == 0)
        {
            installedModules[x] = moduleID;
            break;
        }
    }
}

void HTCOM::updateModule(byte moduleID, ModuleState state)
{
    for (byte x = 0; x < MAX_INSTALLED_MODULES; x++)
    {
        if (installedModules[x] == 0)
            break;
        if (installedModules[x] == moduleID)
        {
            stateOfModules[x] = state;
            break;
        }
    }
}

bool HTCOM::isModuleState(byte moduleID, ModuleState state)
{
    for (byte x = 0; x < MAX_INSTALLED_MODULES; x++)
    {
        if (installedModules[x] == 0)
            break;
        if (installedModules[x] == moduleID)
            return stateOfModules[x] == (byte)state;
    }
    return false;
}

bool HTCOM::isAllResolved()
{
    for (byte x = 0; x < MAX_INSTALLED_MODULES; x++)
    {
        if (installedModules[x] == 0)
            break;
        if (stateOfModules[x] != ModuleState::DISARMED)
            return false;
    }
    return true;
}

byte HTCOM::installedModuleCount()
{
    byte count = 0;
    for (byte x = 0; x < MAX_INSTALLED_MODULES; x++)
    {
        if (installedModules[x] == 0)
            break;
        count++;
    }
    return count;
}

byte HTCOM::getInstalledModuleID(byte idx)
{
    if (idx >= MAX_INSTALLED_MODULES)
    {
        return ID_NONE;
    }
    return installedModules[idx];
}

bool HTCOM::isBeep()
{
    if (toBeep) {
        toBeep = false;
        return true;
    }
    return false;
}

void HTCOM::addTestModule()
{
    addToModuleList(ID_MAZE);
}

#endif

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

void HTCOM::setGameDifficulty(byte dif)
{
    difficulty = dif;
}

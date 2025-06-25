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
    m_moduleID = id;
}

void HTCOM::attach(uint8_t id)
{
    m_mcp2515 = new MCP2515(CS_PIN);
    m_gametime = 3600;
    m_moduleID = id;
#ifndef HI_MODULE
    resetCtrlError();
    initModules();
#endif
    m_newAmbSettings = false;
    m_newGameSettings = false;
    m_difficulty = 0; // Difficulty::SIMPLE
    m_newStrike = false;
    m_paused = false;
    m_strikes = 0;
    m_brightness = DEFAULT_BRIGHTNESS;

    m_mcp2515->reset();
    m_mcp2515->setBitrate(CAN_500KBPS);
    m_mcp2515->setNormalMode();
}

#ifndef HI_MODULE
void HTCOM::initModules()
{
    for (byte x = 0; x < MAX_INSTALLED_MODULES; x++)
    {
        m_installedModules[x] = 0;
        m_stateOfModules[x] = ModuleState::UNKNOWN;
    }
}
#endif

void HTCOM::poll()
{
    if (m_mcp2515->readMessage(&m_rcvCanMsg) == MCP2515::ERROR_OK)
    {
        unsigned long canID = m_rcvCanMsg.can_id;
        dbgOut(F("msg: 0x"));
        dbgOutLn2(canID, HEX);
        byte rcvModule = ID_CONTROLLER;
        if (canID > 0x0000ff)
        {
            rcvModule = canID & 0x0000ff;
            canID = canID & 0x00ff00;
        }
        if (canID == MID_BEEP)
        {
            rcvModule = ID_MAX_MODULES;
        }
        if (m_moduleID == ID_CONTROLLER)
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
            dbgOut(F("hb: "));
            m_paused = false;
            m_gametime = (m_rcvCanMsg.data[0] << 8) + m_rcvCanMsg.data[1];
            {
                byte strikes = m_rcvCanMsg.data[2];
                if (strikes != m_strikes)
                {
                    m_newStrike = true;
                }
                m_strikes = strikes;
            }
            dbgOut(m_gametime);
            dbgOut(F(" s:"));
            dbgOutLn(m_strikes);
            break;
        case MID_AMBIENTSETTINGS:
            m_brightness = (m_rcvCanMsg.data[0]);
            m_newAmbSettings = true;
            break;
        case MID_GAMESETTINGS:
            m_difficulty = m_rcvCanMsg.data[0];
            m_inds = m_rcvCanMsg.data[1] + (m_rcvCanMsg.data[2] << 8);
            m_snr = uint32_t(m_rcvCanMsg.data[3]) + (uint32_t(m_rcvCanMsg.data[4]) << 8) + (uint32_t(m_rcvCanMsg.data[5]) << 16);
            m_newGameSettings = true;

            dbgOut("gs:");
            dbgOut2(m_difficulty, HEX);
            dbgOut(" ");
            dbgOut2(m_inds, HEX);
            dbgOut(" ");
            dbgOut2(m_rcvCanMsg.data[3], HEX);
            dbgOut(" ");
            dbgOut2(m_rcvCanMsg.data[4], HEX);
            dbgOut(" ");
            dbgOut2(m_rcvCanMsg.data[5], HEX);
            dbgOut(" ");
            dbgOutLn2(m_snr, HEX);
            break;
        case MID_GAME_PAUSED:
            m_paused = true;
            dbgOut(F("p:"));
            break;
#ifndef HI_MODULE
        case MID_ERROR:
            setCtrlError(m_rcvCanMsg.data[0]);
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
            dbgOutLn2(m_rcvCanMsg.data[1], DEC);
            updateModule(rcvModule, static_cast<ModuleState>(m_rcvCanMsg.data[1]));
            break;
        case MID_BEEP:
            dbgOutLn(F("beep"));
            m_toBeep = true;
            break;
        case MID_STRIKE:
            dbgOutLn(F("s"));
            if (!m_paused)
            {
                m_strikes++;
                m_newStrike = true;
            }
            break;
#endif
        default:
            dbgOut(F("unk: "));
            dbgOut2(rcvModule, DEC);
            dbgOut(F(":"));
            dbgOutLn2(m_rcvCanMsg.can_id, DEC);
            break;
        };
    }
#ifndef HI_MODULE
    // check if there is an message incoming
    if (m_hasError && (millis() > m_errTime))
        resetCtrlError();
#endif
}

uint32_t HTCOM::getSerialNumber()
{
    return m_snr;
}

word HTCOM::getIndicators()
{
    return m_inds;
}

void HTCOM::sendModuleID()
{
    byte dl = m_moduleID - ID_CONTROLLER;
    delay(dl * 100);
    m_sndCanMsg.can_id = MID_MODULEINIT + m_moduleID;
    m_sndCanMsg.can_dlc = 1;
    m_sndCanMsg.data[0] = m_moduleID;

    m_mcp2515->sendMessage(&m_sndCanMsg);
}

void HTCOM::sendError(byte err)
{
    m_sndCanMsg.can_id = MID_ERROR + m_moduleID;
    m_sndCanMsg.can_dlc = 1;
    m_sndCanMsg.data[0] = err;

    m_mcp2515->sendMessage(&m_sndCanMsg);
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
    m_sndCanMsg.can_id = MID_STATE + m_moduleID;
    m_sndCanMsg.can_dlc = 2;
    m_sndCanMsg.data[0] = m_moduleID;
    m_sndCanMsg.data[1] = state;

    m_mcp2515->sendMessage(&m_sndCanMsg);
}

void HTCOM::sendStrike()
{
    m_sndCanMsg.can_id = MID_STRIKE + m_moduleID;
    m_sndCanMsg.can_dlc = 1;
    m_sndCanMsg.data[0] = m_moduleID;

    m_mcp2515->sendMessage(&m_sndCanMsg);
}

void HTCOM::sendBeep()
{
    m_sndCanMsg.can_id = MID_BEEP;
    m_sndCanMsg.can_dlc = 1;
    m_sndCanMsg.data[0] = m_moduleID;

    m_mcp2515->sendMessage(&m_sndCanMsg);
}

byte HTCOM::getBrightness()
{
    return m_brightness;
}

byte HTCOM::getStrikes()
{
    return m_strikes;
}

bool HTCOM::hasNewStrikes()
{
    if (m_newStrike)
    {
        m_newStrike = false;
        return true;
    }
    return false;
}

int HTCOM::getGameTime()
{
    return m_gametime;
}

#ifndef HI_MODULE
void HTCOM::setCtrlSerialNumber(uint32_t sn)
{
    dbgOut(F("snr: "));
    dbgOutLn2(sn, HEX);
    m_snr = sn;
}

void HTCOM::sendCtrlHearbeat(word countdown)
{
    m_sndCanMsg.can_id = MID_HEARTBEAT;
    m_sndCanMsg.can_dlc = 3;
    m_sndCanMsg.data[0] = countdown >> 8;
    m_sndCanMsg.data[1] = countdown & 0x00FF;
    m_sndCanMsg.data[2] = m_strikes;

    m_mcp2515->sendMessage(&m_sndCanMsg);
}

void HTCOM::setCtrlIndicators(word inds)
{
    m_inds = inds;
}

void HTCOM::setCtlrStrikes(byte strikes)
{
    m_strikes = strikes;
}

void HTCOM::setCtrlDifficulty(byte difficulty)
{
    dbgOut("df: ");
    dbgOutLn2(difficulty, HEX);
    m_difficulty = difficulty;
}

void HTCOM::setCtrlBrightness(byte brightness)
{
    m_brightness = brightness;
    sendCtrlAmbientSettings();
}

// the game is paused, will continue with a new heartbeat
void HTCOM::setCtrlGamePaused(bool paused)
{
    m_paused = paused;
    if (paused)
    {
        m_sndCanMsg.can_id = MID_GAME_PAUSED;
        m_sndCanMsg.can_dlc = 1;
        m_sndCanMsg.data[0] = 0x00;

        m_mcp2515->sendMessage(&m_sndCanMsg);
    }
}

// game settings will be sendet to all modules
void HTCOM::sendCtrlGameSettings()
{
    m_sndCanMsg.can_id = MID_GAMESETTINGS;
    m_sndCanMsg.can_dlc = 6;
    m_sndCanMsg.data[0] = m_difficulty;
    m_sndCanMsg.data[1] = m_inds & 0x00FF;
    m_sndCanMsg.data[2] = m_inds >> 8;
    m_sndCanMsg.data[3] = m_snr & 0xff;
    m_sndCanMsg.data[4] = (m_snr >> 8) & 0xff;
    m_sndCanMsg.data[5] = (m_snr >> 16) & 0xff;

    dbgOut("gs:");
    byte difficulty = m_sndCanMsg.data[0];
    dbgOut2(difficulty, HEX);
    uint16_t inds = m_sndCanMsg.data[1] + (m_sndCanMsg.data[2] << 8);
    dbgOut(" ");
    dbgOut2(inds, HEX);
    dbgOut(" ");
    dbgOut2(m_sndCanMsg.data[3], HEX);
    dbgOut(" ");
    dbgOut2(m_sndCanMsg.data[4], HEX);
    dbgOut(" ");
    dbgOut2(m_sndCanMsg.data[5], HEX);
    dbgOut(" ");
    dbgOutLn2(m_snr, HEX);

    m_mcp2515->sendMessage(&m_sndCanMsg);
    m_toBeep = false;
}

// here all ambient settings will be sended to all modules
void HTCOM::sendCtrlAmbientSettings()
{
    m_sndCanMsg.can_id = MID_AMBIENTSETTINGS;
    m_sndCanMsg.can_dlc = 1;
    m_sndCanMsg.data[0] = m_brightness;

    m_mcp2515->sendMessage(&m_sndCanMsg);
}

// this will be sended to evaluate which modules are installed.
void HTCOM::sendCtrlInitialisation()
{
    m_sndCanMsg.can_id = MID_INITIALISATION;
    m_sndCanMsg.can_dlc = 1;
    m_sndCanMsg.data[0] = 0xff;

    m_mcp2515->sendMessage(&m_sndCanMsg);
}

void HTCOM::setCtrlError(byte error)
{
    m_lastError = error;
    m_hasError = true;
    m_errTime = millis() + 5000;
}

void HTCOM::resetCtrlError()
{
    m_errTime = 0;
    m_hasError = false;
    m_lastError = 0;
}

bool HTCOM::hasCtrlError()
{
    return m_hasError;
}

byte HTCOM::getCtrlError()
{
    return m_lastError;
}

void HTCOM::addToModuleList(byte moduleID)
{
    for (byte x = 0; x < MAX_INSTALLED_MODULES; x++)
    {
        // module already installed ?
        if (m_installedModules[x] == moduleID)
        {
            break;
        }
        // free space
        if (m_installedModules[x] == 0)
        {
            m_installedModules[x] = moduleID;
            break;
        }
    }
}

void HTCOM::updateModule(byte moduleID, ModuleState state)
{
    for (byte x = 0; x < MAX_INSTALLED_MODULES; x++)
    {
        if (m_installedModules[x] == 0)
            break;
        if (m_installedModules[x] == moduleID)
        {
            m_stateOfModules[x] = state;
            break;
        }
    }
}

bool HTCOM::isModuleState(byte moduleID, ModuleState state)
{
    for (byte x = 0; x < MAX_INSTALLED_MODULES; x++)
    {
        if (m_installedModules[x] == 0)
            break;
        if (m_installedModules[x] == moduleID)
            return m_stateOfModules[x] == (byte)state;
    }
    return false;
}

bool HTCOM::isAllResolved()
{
    for (byte x = 0; x < MAX_INSTALLED_MODULES; x++)
    {
        if (m_installedModules[x] == 0)
            break;
        if (m_stateOfModules[x] != ModuleState::DISARMED)
            return false;
    }
    return true;
}

byte HTCOM::installedModuleCount()
{
    byte count = 0;
    for (byte x = 0; x < MAX_INSTALLED_MODULES; x++)
    {
        if (m_installedModules[x] == 0)
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
    return m_installedModules[idx];
}

bool HTCOM::isBeep()
{
    if (m_toBeep)
    {
        m_toBeep = false;
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
    if (m_newAmbSettings)
    {
        m_newAmbSettings = false;
        return true;
    }
    return false;
}

bool HTCOM::isNewGameSettings()
{
    if (m_newGameSettings)
    {
        m_newGameSettings = false;
        return true;
    }
    return false;
}

bool HTCOM::isPaused() {
    return m_paused;
}

byte HTCOM::getDifficulty()
{
    return m_difficulty;
}

void HTCOM::setGameDifficulty(byte dif)
{
    m_difficulty = dif;
}

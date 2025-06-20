
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>
#include <globals.h>

const byte DEFAULT_BRIGHTNESS = 4;
const byte CS_PIN = 10;
const byte MAX_INSTALLED_MODULES = 12;

enum ModuleState
{
    UNKNOWN = 0,
    INIT,
    ARMED,
    STRIKED,
    DISARMED
};

enum MODULEIDS
{
    ID_NONE = 0,
    ID_CONTROLLER = 44,
    ID_WIRES = 45,
    ID_MAZE = 46,
    ID_SIMON = 47,
    ID_PASSWORD = 48,
    ID_MORSE = 49,
    ID_NGAS = 50,
    ID_SYMBOLS = 51,
    ID_MAX_MODULES = 52
};

const byte MOD_OFFSET = ID_WIRES;
const char mn_wr[] PROGMEM = "wr";
const char mn_mz[] PROGMEM = "mz";
const char mn_si[] PROGMEM = "si";
const char mn_pw[] PROGMEM = "pw";
const char mn_mr[] PROGMEM = "mo";
const char mn_ng[] PROGMEM = "ng";
const char mn_sy[] PROGMEM = "sy";
const char *const MODULE_LABELS[] PROGMEM = {mn_wr, mn_mz, mn_si, mn_pw, mn_mr, mn_ng, mn_sy};

const byte MODULE_COUNT = 8;
const byte modules[MODULE_COUNT] = {ID_CONTROLLER, ID_WIRES, ID_MAZE, ID_SIMON, ID_PASSWORD, ID_MORSE, ID_NGAS, ID_SYMBOLS};

const char e_nn[] PROGMEM = "";
const char e_we[] PROGMEM = lb_err_invalid_wires;

const char *const ERROR_MESSAGES[] PROGMEM = {e_nn, e_we};

enum COMMANDS
{
    MID_HEARTBEAT = 0x0001,
    MID_AMBIENTSETTINGS = 0x0002,
    MID_GAMESETTINGS = 0x0003,
    MID_INITIALISATION = 0x0004,
    MID_GAME_PAUSED = 0x0005,
    MID_ERROR = 0x0300,
    MID_STATE = 0x0400,
    MID_STRIKE = 0x0500,
    MID_MODULEINIT = 0x0600,
    MID_BEEP = 0x0010
};

enum PARAMETER
{
    PAR_DISARM = 1,
    PAR_ARM,
};

class HTCOM
{
public:
    HTCOM(byte id);
    HTCOM();

    void attach(byte id);
    void poll();

    // module functions
    void sendArmed();
    void sendError(byte err);
    void sendDisarmed();
    void sendStrike();
    int getGameTime();
    byte getDifficulty();
    uint16_t getIndicators();

    bool hasNewStrikes();
    byte getStrikes();
    byte getBrightness();
    uint32_t getSerialNumber();
    bool isNewAmbSettings();
    bool isNewGameSettings();
    bool isPaused();

    void setGameDifficulty(byte dif);
    void sendModuleID();
    void sendBeep();

// Controller only functions
#ifndef HI_MODULE
    void setCtlrStrikes(byte strikes);
    void setCtrlSerialNumber(uint32_t srn);
    void setCtrlDifficulty(byte difficulty);
    void setCtrlBrightness(byte brightness);
    void setCtrlIndicators(word inds);
    void setCtrlGamePaused(bool paused);

    void sendCtrlHearbeat(word countdown);
    void sendCtrlGameSettings();
    void sendCtrlAmbientSettings();
    void sendCtrlInitialisation();
    void resetCtrlError();

    bool hasCtrlError();
    byte getCtrlError();
    void setCtrlError(byte error);

    bool isAllResolved();

    void initModules();
    byte installedModuleCount();
    byte getInstalledModuleID(byte idx);
    bool isModuleState(byte moduleID, ModuleState state);

    bool isBeep();
    void addTestModule();

#endif

protected:
    void sendModuleState(ModuleState state);
    byte m_moduleID;

    int m_gametime;
    word m_inds;

    bool m_newStrike;
    byte m_strikes;
    byte m_difficulty;

    bool m_newAmbSettings;  // new ambient settings received
    bool m_newGameSettings; // new game settings received
    byte m_brightness;

    bool m_hasError;
    byte m_lastError;
    unsigned long m_errTime;

    uint32_t m_snr;

    can_frame m_sndCanMsg;
    can_frame m_rcvCanMsg;
    MCP2515 *m_mcp2515;

#ifndef HI_MODULE
    void addToModuleList(byte moduleID);
    void updateModule(byte moduleID, ModuleState state);

    bool m_toBeep;
    byte m_installedModules[MAX_INSTALLED_MODULES];
    byte m_stateOfModules[MAX_INSTALLED_MODULES];

    bool m_paused;
#endif
};

#endif
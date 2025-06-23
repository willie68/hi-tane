#ifndef GLOBALS_H
#define GLOBALS_H

#define deutsch
// #define english

#define lb_version "V0.2a"
#define lb_game_name "HI-Tane"

#ifdef deutsch
#define lb_welcome "Willkommen"
#define lb_start_game "Spiel starten?"
#define lb_difficulty "Schwierigkeit"
#define lb_brigthness "Helligkeit"
#define lb_gametime "Spielzeit"
#define lb_paused "Spielpause"

#define lb_yes "Ja"
#define lb_gamemode_simple "Einfach"
#define lb_gamemode_medium "Mittel "
#define lb_gamemode_hard "Schwer "
#define lb_short_minutes " min"

#define lb_err_invalid_wires "wires: falsche Kabel"

#define lb_resolved_1 "Prima, die Bombe"
#define lb_resolved_2 "ist entsch�rft."
#define lb_new_game "neues Spiel? Ja"

#define lb_striked_1 "Die Bombe"
#define lb_striked_2 "ist explodiert."

#define lb_pause_return "weiter"
#define lb_pause_reset "abbrechen"

#endif

#ifdef english
#define lb_welcome "Welcome"
#define lb_start_game "start game"
#define lb_difficulty "Difficulty"
#define lb_brigthness "Brigthness"
#define lb_gametime "Game time"
#define lb_paused "Game paused"

#define lb_yes "yes"
#define lb_gamemode_simple "Easy  "
#define lb_gamemode_medium "Medium"
#define lb_gamemode_hard "Hard  "
#define lb_short_minutes " min"

#define lb_err_invalid_wires "wires: invalid wire"

#define lb_resolved_1 "Hurray, you"
#define lb_resolved_2 "defused the bomb."
#define lb_resolved_3 "Another game? yes"

#define lb_striked_1 "sorry, you exploded!"
#define lb_striked_2 ""

#define lb_pause_return "return"
#define mk_pause_reset "cancel"

#endif

// strings for english

// Menu constants
const char mk_start_game[] PROGMEM = lb_start_game;
const char mk_difficulty[] PROGMEM = lb_difficulty;
const char mk_brigthness[] PROGMEM = lb_brigthness;
const char mk_gametime[] PROGMEM = lb_gametime;
const char mk_paused[] PROGMEM = lb_paused;

PGM_P const MENU_KEYS[] PROGMEM = {
    mk_start_game,
    mk_difficulty,
    mk_brigthness,
    mk_gametime,
    mk_paused};

const char mk_pause_return[] PROGMEM = lb_pause_return;
const char mk_pause_reset[] PROGMEM = lb_pause_reset;
PGM_P const PAUSE_MENU_VALUES[] PROGMEM = {
    mk_pause_return,
    mk_pause_reset};

#endif
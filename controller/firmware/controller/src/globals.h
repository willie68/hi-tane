#ifndef GLOBALS_H
#define GLOBALS_H
#include <game.h>

#define Version "V 0.2" // always add an space at the end

// Menu constants
const char mk_start_game[] PROGMEM = "start Game?";
const char mk_difficulty[] PROGMEM = "Difficulty";
const char mk_brigthness[] PROGMEM = "Brigthness";
const char mk_gametime[] PROGMEM = "Gametime";

PGM_P const MENU_KEYS[] PROGMEM = {
    mk_start_game,
    mk_difficulty,
    mk_brigthness,
    mk_gametime
};

#endif
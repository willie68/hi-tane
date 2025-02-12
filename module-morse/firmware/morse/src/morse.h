#ifndef MORSE_H
#define MORSE_H

#include <Arduino.h>

#define DIGIT_START 26
#define UMLAUT_START 36
#define NO_PIN 255

// Morse Code (explicit declaration of letter timings)
const uint8_t MORSE_LETTERS[40] = {
    /* a */ B101,
    /* b */ B11000,
    /* c */ B11010,
    /* d */ B1100,
    /* e */ B10,
    /* f */ B10010,
    /* g */ B1110,
    /* h */ B10000,
    /* i */ B100,
    /* j */ B10111,
    /* k */ B1101,
    /* l */ B10100,
    /* m */ B111,
    /* n */ B110,
    /* o */ B1111,
    /* p */ B10110,
    /* q */ B11101,
    /* r */ B1010,
    /* s */ B1000,
    /* t */ B11,
    /* u */ B1001,
    /* v */ B10001,
    /* w */ B1011,
    /* x */ B11001,
    /* y */ B11011,
    /* z */ B11100,
    /* 0 */ B111111,
    /* 1 */ B101111,
    /* 2 */ B100111,
    /* 3 */ B100011,
    /* 4 */ B100001,
    /* 5 */ B100000,
    /* 6 */ B110000,
    /* 7 */ B111000,
    /* 8 */ B111100,
    /* 9 */ B111110,
    /* Ä */ B10101,
    /* Ö */ B11110,
    /* Ü */ B10011,
    /* ß */ B10001100};

class Morse
{
public:
    Morse(byte ledPin, byte tonePin);

private:
    byte ledP;
    byte toneP;
};

#endif
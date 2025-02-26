#ifndef WORDS_H
#define WORDS_H

#include <Arduino.h>

word frqBase = 3500; // MHz

struct WordFrequency
{
  String word;
  uint16_t frq;
};

const WordFrequency w0 = WordFrequency{"halle", 3505};
const WordFrequency w1 = WordFrequency{"hallo", 3515};
const WordFrequency w2 = WordFrequency{"lokal", 3522};
const WordFrequency w3 = WordFrequency{"steak", 3532};
const WordFrequency w4 = WordFrequency{"strom", 3535};
const WordFrequency w5 = WordFrequency{"speck", 3542};
const WordFrequency w6 = WordFrequency{"bistro", 3545};
const WordFrequency w7 = WordFrequency{"robust", 3552};
const WordFrequency w8 = WordFrequency{"sehne", 3555};
const WordFrequency w9 = WordFrequency{"knopf", 3565};
const WordFrequency wa = WordFrequency{"sektor", 3572};
const WordFrequency wb = WordFrequency{"vektor", 3575};
const WordFrequency wc = WordFrequency{"bravo", 3582};
const WordFrequency wd = WordFrequency{"kobra", 3592};
const WordFrequency we = WordFrequency{"bombe", 3595};
const WordFrequency wf = WordFrequency{"norden", 3600};

const byte WORD_COUNT = 16;

const WordFrequency wordset[WORD_COUNT] = {
    w0, w1, w2, w3,
    w4, w5, w6, w7,
    w8, w9, wa, wb,
    wc, wd, we, wf};

#endif
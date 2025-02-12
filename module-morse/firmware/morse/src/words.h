#ifndef WORDS_H
#define WORDS_H

#include <Arduino.h>

word frqBase = 3500; // MHz

struct WordFrequency
{
  String word;
  byte frq;
};

const WordFrequency w0 = WordFrequency{"halle", byte(3505 - frqBase)};
const WordFrequency w1 = WordFrequency{"hallo", byte(3.515 - frqBase)};
const WordFrequency w2 = WordFrequency{"lokal", byte(3.522 - frqBase)};
const WordFrequency w3 = WordFrequency{"steak", byte(3.532 - frqBase)};
const WordFrequency w4 = WordFrequency{"st\xfc""ck", byte(3.535 - frqBase)};
const WordFrequency w5 = WordFrequency{"speck", byte(3.542 - frqBase)};
const WordFrequency w6 = WordFrequency{"bistro", byte(3.545 - frqBase)};
const WordFrequency w7 = WordFrequency{"robust", byte(3552 - frqBase)};
const WordFrequency w8 = WordFrequency{"s\xe4""bel", byte(3555 - frqBase)};
const WordFrequency w9 = WordFrequency{"s\xfc""lze", byte(3565 - frqBase)};
const WordFrequency wa = WordFrequency{"sektor", byte(3572 - frqBase)};
const WordFrequency wb = WordFrequency{"vektor", byte(3575 - frqBase)};
const WordFrequency wc = WordFrequency{"bravo", byte(3582 - frqBase)};
const WordFrequency wd = WordFrequency{"kobra", byte(3592 - frqBase)};
const WordFrequency we = WordFrequency{"bombe", byte(3595 - frqBase)};
const WordFrequency wf = WordFrequency{"s\xfc""den", byte(3600 - frqBase)};

const byte WORD_COUNT = 16;

const WordFrequency wordset[WORD_COUNT] = {
    w0, w1, w2, w3,
    w4, w5, w6, w7,
    w8, w9, wa, wb,
    wc, wd, we, wf};

#endif
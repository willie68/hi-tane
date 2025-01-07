#ifndef INDICATORS_H
#define INDICATORS_H
const uint8_t INDICATOR_COUNT = 11;

// defining the flags
enum INDICATOR {
    SND = 1,
    CLR,
    CAR,
    IND,
    FRQ,
    SIG, 
    NSA, 
    MSA,
    TRN,
    BOB, 
    FRK
};

const char nn[] PROGMEM = "NN"; 
const char snd[] PROGMEM = "SND"; 
const char clr[] PROGMEM = "CLR"; 
const char car[] PROGMEM = "CAR"; 
const char ind[] PROGMEM = "IND"; 
const char frq[] PROGMEM = "FRQ";   
const char sig[] PROGMEM = "SIG"; 
const char nsa[] PROGMEM = "NSA"; 
const char msa[] PROGMEM = "MSA"; 
const char trn[] PROGMEM = "TRN"; 
const char bob[] PROGMEM = "BOB"; 
const char frk[] PROGMEM = "FRK";

const char* const INDICATORNAMES[] PROGMEM = { 
nn, snd, clr, car, ind, frq, sig, nsa, msa, trn, bob, frk
};
#endif
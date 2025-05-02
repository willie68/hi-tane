#ifndef MORSE_H
#define MORSE_H

#include <Arduino.h>

#define DIGIT_START 26
#define UMLAUT_START 36
#define NO_PIN 255

const word mFreq = 400;

const byte alphabet[] = {
    0x41, // A
    0x88, // B
    0x8A, // C
    0x64, // D
    0x20, // E
    0x82, // F
    0x66, // G
    0x80, // H
    0x40, // I
    0x87, // J
    0x65, // K
    0x84, // L
    0x43, // M
    0x42, // N
    0x67, // O
    0x86, // P
    0x8D, // Q
    0x62, // R
    0x60, // S
    0x21, // T
    0x61, // U
    0x81, // V
    0x63, // W
    0x89, // X
    0x8B, // Y
    0x8C, // Z
    0xBF, // 0
    0xAF, // 1
    0xA7, // 2
    0xA3, // 3
    0xA1, // 4
    0xA0, // 5
    0xB0, // 6
    0xB8, // 7
    0xBC, // 8
    0xBE, // 9
};

class Morse
{
public:
    Morse(byte ledPin, byte tonePin, long dit = 150);
    void setDitLength(word millis);
    word getDitLength();

    void sendMessage(const char *msg);
    void poll();
    void reset();
    bool finished();

private:
    void calculateLengths();

    bool _bls;
    uint8_t _ls; // pin to create a tone
    bool _bled;
    uint8_t _led;     // pin to show code with led
    word _dit;        // ms length of dit
    const char *_msg; // message to send
    uint8_t _pos;     // pos in message
    bool _cend;       // end of char
    bool _mend;       // end of morse message
    uint8_t _ch;      // the actual char to send
    bool _mit;        // we are in mit (morse bit) time
    bool _pse;        // we are in pause time
    long _t;          // time of the next mit, or pause to begin
    uint8_t _mcnt;    // anzahl der mit in dem zeichen
    uint8_t _mdta;    // bit array mit den daten der mit's
    uint8_t _mpos;    // position des aktuellen mit

    word _dah;
    word _symPause;
    word _charPause;
    word _wordPause;
};

#endif
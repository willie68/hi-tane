#ifndef SHIFT7SDIGIT_H
#define SHIFT7SDIGIT_H
#include <Arduino.h>

enum Segment
{
    A = 1,
    B = 2,
    C = 4,
    D = 8,
    E = 16,
    F = 32,
    G = 64,
    DP = 128
};

const byte Numbers[16] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // B
    0x39, // C
    0x5E, // D
    0x79, // E
    0x71, // F
};

const byte MAX_DIGITS = 8;

class Shift7Segment
{
public:
    Shift7Segment(byte num, byte data, byte clock, byte latch, bool invert = false);
    void invert(bool invert);
    void showNumber(int value);
    void showNumberHex(int value);
    void showDigit(byte pos, byte value);
    void clear();
    void showDot(byte pos, bool show);
    void showSegments(byte pos, byte segments);

private:
    byte digits;
    byte pin_data;
    byte pin_clk;
    byte pin_latch;
    bool inv;

    byte buf[MAX_DIGITS];
    bool dps[MAX_DIGITS];
    void updateShiftRegister();
    byte dec2Digit(byte value);
    void clearDots();
    void clearValue();
};
#endif
#include <Arduino.h>
#include <shift7sDigit.h>

Shift7Segment::Shift7Segment(byte num, byte data, byte clock, byte latch)
{
    this->digits = num;
    this->pin_data = data;
    this->pin_clk = clock;
    this->pin_latch = latch;

    pinMode(pin_data, OUTPUT);
    pinMode(pin_clk, OUTPUT);
    pinMode(pin_latch, OUTPUT);
}

void Shift7Segment::showNumber(int value)
{
    clear();
    byte pos = 0;
    word av = abs(value);
    while (av > 0)
    {
        showDigit(pos++, byte(av % 10));
        av = av / 10;
    }
    if (value < 0)
        showSegments(pos, 0x40);
    updateShiftRegister();
};

void Shift7Segment::showNumberHex(int value)
{
    clear();
    byte pos = 0;
    word av = abs(value);
    while (av > 0)
    {
        showDigit(pos++, byte(av & 0xf));
        av = av >> 4;
    }
    if (value < 0)
        showSegments(pos, 0x40);
    updateShiftRegister();
};

void Shift7Segment::showDigit(byte pos, byte value)
{
    if (pos < MAX_DIGITS)
    {
        buf[pos] = dec2Digit(value);
    }
};

byte Shift7Segment::dec2Digit(byte value)
{
    if (value < 16)
    {
        return Numbers[value];
    }
    return 0;
}

void Shift7Segment::clear()
{
    for (byte x = 0; x < MAX_DIGITS; x++)
    {
        buf[x] = 0x00;
    }
    updateShiftRegister();
};

void Shift7Segment::showDot(byte pos, bool show)
{
    if (show)
    {
        buf[pos] |= 0x80;
    }
    else
    {
        buf[pos] &= 0x7F;
    }
    updateShiftRegister();
};

void Shift7Segment::showSegments(byte pos, byte segments)
{
    buf[pos] = segments;
    updateShiftRegister();
};

void Shift7Segment::updateShiftRegister()
{
    digitalWrite(pin_latch, LOW);
    for (byte x = 0; x < digits; x++)
        shiftOut(pin_data, pin_clk, MSBFIRST, buf[x]);
    digitalWrite(pin_latch, HIGH);
}

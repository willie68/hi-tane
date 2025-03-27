#include <Arduino.h>
#include <shift7sDigit.h>

Shift7Segment::Shift7Segment(byte num, byte data, byte clock, byte latch, bool invert)
{
    this->digits = num;
    this->pin_data = data;
    this->pin_clk = clock;
    this->pin_latch = latch;
    this->inv = invert;

    pinMode(pin_data, OUTPUT);
    pinMode(pin_clk, OUTPUT);
    pinMode(pin_latch, OUTPUT);
    clear();
}


void Shift7Segment::invert(bool invert)
{
    inv = invert;
}

void Shift7Segment::showNumber(int value)
{
    clearValue();
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
    clearValue();
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
        buf[pos] = dec2Digit(value);
};

byte Shift7Segment::dec2Digit(byte value)
{
    if (value < 16)
        return Numbers[value];
    return 0;
}

void Shift7Segment::clear()
{
    clearValue();
    clearDots();
    updateShiftRegister();
};

void Shift7Segment::clearValue()
{
    for (byte x = 0; x < digits; x++)
        buf[x] = 0x00;
};

void Shift7Segment::clearDots()
{
    for (byte x = 0; x < digits; x++)
        dps[x] = false;
};

void Shift7Segment::showDot(byte pos, bool show)
{
    dps[pos] = show;
    updateShiftRegister();
};

void Shift7Segment::showSegments(byte pos, byte segments)
{
    buf[pos] = segments;
    updateShiftRegister();
};

void Shift7Segment::updateShiftRegister()
{
    byte segs = 0;
    digitalWrite(pin_latch, LOW);
    for (byte x = 0; x < digits; x++)
    {
        segs = buf[x];
        if (dps[x])
            segs |= 0x80;
        else
            segs &= 0x7F;
        if (inv) {
            segs =~segs;
        }

        shiftOut(pin_data, pin_clk, MSBFIRST, segs);
    }
    digitalWrite(pin_latch, HIGH);
}

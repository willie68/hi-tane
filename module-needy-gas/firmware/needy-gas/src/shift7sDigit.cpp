#include <Arduino.h>
#include <shift7sDigit.h>

Shift7Segment::Shift7Segment(byte num, byte data, byte clock, byte latch, bool invert)
{
    this->m_digits = num;
    this->m_pin_data = data;
    this->m_pin_clk = clock;
    this->m_pin_latch = latch;
    this->m_inv = invert;

    pinMode(m_pin_data, OUTPUT);
    pinMode(m_pin_clk, OUTPUT);
    pinMode(m_pin_latch, OUTPUT);
    clear();
}


void Shift7Segment::invert(bool invert)
{
    m_inv = invert;
}

void Shift7Segment::showNumber(int value)
{
    clearValue();
    byte pos = 0;
    word av = abs(value);
    if (av == 0) {
        showDigit(pos, 0);
    }
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
    m_buf[pos] = dec2Digit(value);
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
    for (byte x = 0; x < m_digits; x++)
    m_buf[x] = 0x00;
};

void Shift7Segment::clearDots()
{
    for (byte x = 0; x < m_digits; x++)
        m_dps[x] = false;
};

void Shift7Segment::showDot(byte pos, bool show)
{
    m_dps[pos] = show;
    updateShiftRegister();
};

void Shift7Segment::showSegments(byte pos, byte segments)
{
    m_buf[pos] = segments;
    updateShiftRegister();
};

void Shift7Segment::updateShiftRegister()
{
    byte segs = 0;
    digitalWrite(m_pin_latch, LOW);
    for (byte x = 0; x < m_digits; x++)
    {
        segs = m_buf[x];
        if (m_dps[x])
            segs |= 0x80;
        else
            segs &= 0x7F;
        if (m_inv) {
            segs =~segs;
        }

        shiftOut(m_pin_data, m_pin_clk, MSBFIRST, segs);
    }
    digitalWrite(m_pin_latch, HIGH);
}

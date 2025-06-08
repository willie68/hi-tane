#ifndef INDICATORS_H
#define INDICATORS_H

// #define debug

const uint8_t INDICATOR_COUNT = 11;

// defining the flags
enum INDICATOR
{
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

const char *const INDICATORNAMES[] PROGMEM = {nn, snd, clr, car, ind, frq, sig, nsa, msa, trn, bob, frk};

class Indicators
{
public:
    Indicators()
    {
        for (byte i = 0; i < 3; i++)
        {
            indicators[i] = 0;
            indicatorsActive[i] = false;
        }
    }

    byte Count()
    {
        return count;
    }

    void Generate()
    {
        byte indCount = random(1, 4);
        for (byte x = 0; x < indCount; x++)
        {
            Add(random(INDICATOR_COUNT) + 1, random(2) == 0);
        }
    }

    void Add(byte ind, bool active)
    {
        if (count < 3)
        {
            indicators[count] = ind;
            indicatorsActive[count] = active;
            count++;
        }
    }

    word Compress()
    {
        return indicators[0] | indicators[1] << 4 | indicators[2] << 8 | indicatorsActive[0] << 12 | indicatorsActive[1] << 13 | indicatorsActive[2] << 14;
    }

    void Decompress(word value)
    {
        indicators[0] = value & 0x000F;
        indicators[1] = (value & 0x00F0) >> 4;
        indicators[2] = (value & 0x0F00) >> 8;
        indicatorsActive[0] = (value & 0x1000) > 0;
        indicatorsActive[1] = (value & 0x2000) > 0;
        indicatorsActive[2] = (value & 0x4000) > 0;
        count = 0;
        for (byte i = 0; i < 3; i++)
            if (indicators[i] > 0)
                count++;
    }

    bool IsActive(byte idx)
    {
        if (idx >= count)
            return false;
        return indicatorsActive[idx];
    }

    byte Get(byte idx)
    {
        if (idx >= count)
            return 0;
        return indicators[idx];
    }

    bool hasIndicator(INDICATOR ind, bool active)
    {
        int8_t pos = -1;
        // check if the indicator is in the list
        for (byte x = 0; x < 3; x++)
        {
            if (indicators[x] == ind)
            {
                pos = x;
            }
        }
        if (pos < 0)
            return false; // indicator not found
        // check if the state is equal
        return (indicatorsActive[pos] == active);
    }

private:
    byte count;
    uint8_t indicators[3];
    bool indicatorsActive[3];
};

#endif
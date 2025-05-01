#ifndef SERIALNUMBER_H
#define SERIALNUMBER_H

#include <Arduino.h>
//                          0123456789abcdef 
const char ch1[] PROGMEM = "AEBCGKMPRTVX";
const char ch2[] PROGMEM = "IUDFHJLNQSWZ";

class SerialNumber
{
public:
    void Generate()
    {
        snr[0] = random(12) << 4 | random(12);
        snr[1] = random(12) << 4 | random(12);
        snr[2] = random(10) << 4 | random(10);
    };
    uint32_t Get()
    {
        return uint32_t(snr[0]) + (uint32_t(snr[1]) << 8) + (uint32_t(snr[2]) << 16);
    };
    
    void String(char *buffer) {
        buffer[0] = pgm_read_byte(&ch1[snr[0] & 0x0f]);
        buffer[1] = pgm_read_byte(&ch2[(snr[0] & 0xf0)>>4]);
        buffer[2] = (snr[2] & 0x0f) + '0';
        buffer[3] = pgm_read_byte(&ch1[snr[1] & 0x0f]);
        buffer[4] = pgm_read_byte(&ch2[(snr[1] & 0xf0)>>4]);
        buffer[5] = (snr[2] >> 4)  + '0';
        buffer[6] = 0x00;
    };

    void Set(uint32_t value)
    {
        snr[0] = value & 0xff;
        snr[1] = (value & 0xff00) >> 8;
        snr[2] = (value & 0xff0000) >> 16;
    }

    bool isVocal() {
        bool vocal = false;
        vocal = vocal || ((snr[0] & 0x0f) < 2) || (((snr[0] & 0xf0)>> 4)< 2);
        vocal = vocal || ((snr[1] & 0x0f) < 2) || (((snr[1] & 0xf0)>> 4)< 2);
        return vocal;
    };

    bool isOdd(){
        return false;
    };

    bool isEven() {
        return false;
    };

private:
    byte snr[3];
};

#endif
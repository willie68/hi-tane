#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

// only one of this should be active
//#define NN
#define LCD
//#define OLED


class HIDisplay : public Print
{
public:
    HIDisplay();
    void init();
    void clear();
    void setCursor(uint8_t column, uint8_t row);

    void noBlink();
    void blink();
    void noCursor();
    void cursor();

    size_t write(uint8_t character);
};

#endif
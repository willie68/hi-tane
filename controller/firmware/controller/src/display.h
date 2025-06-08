#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "indicators.h"
#include "communication.h"
#include "serialnumber.h"

// only one of this should be active
// #define NN
#define LCD
// #define OLED

class HIDisplay : public Print
{
public:
    HIDisplay();
    void init(HTCOM &htcom, Indicators &indicators, SerialNumber &serialnumber);

    void setBrightness(uint8_t brightness);

    void clear();
    void setCursor(uint8_t column, uint8_t row);
    void clearrow(uint8_t row);

    void noBlink();
    void blink();
    void noCursor();
    void cursor();

    void printHeader(bool withVersion);
    void printStatus();
    void printWelcome();
    void showTime(int t);
    void showStrikes();

    void printError(byte err);
    void clearError();

    void resolved();
    void fullyStriked();

    size_t write(uint8_t character);

    void printModules();

private:
    HTCOM *m_htcom;
    Indicators *m_indicators;
    SerialNumber *m_serialnumber;
    uint8_t m_brightness;
};

#endif
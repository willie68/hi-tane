#include <display.h>
#include "Arduino.h"
#define debug
#include <debug.h>

#ifdef NN

HIDisplay::HIDisplay()
{
}

void HIDisplay::init(HTCOM &htcom, Indicators &indicators, SerialNumber &serialnumber)
{
    m_htcom = &htcom;
    m_indicators = &indicators;
    m_serialnumber = &serialnumber;
    Serial.begin(115200);
    Serial.println("----------");
    Serial.println("Welcome");
    Serial.println("----------");
}

void HIDisplay::clear()
{
}

void HIDisplay::setCursor(uint8_t column, uint8_t row)
{
}

void HIDisplay::noBlink()
{
};

void HIDisplay::blink()
{
};

void HIDisplay::noCursor()
{
};

void HIDisplay::cursor()
{
};

size_t HIDisplay::write(uint8_t character)
{
    return 0;
}

#endif

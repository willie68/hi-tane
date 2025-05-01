// Definition of a single plug in the patch panel
#ifndef PLUG_H
#define PLUG_H

#include <wire.h>

class Plug
{
public:
    byte m_number;
    Plug(byte pin);

    bool hasWire();
    Wire initial();

    bool actualHasWire();
    Wire actual();

    int getValue();

private:
    byte m_pin;
    Wire m_installed;
    int m_value;
};

#endif
// Implementation of the patch panel
#include <Arduino.h>
#include <panel.h>
#define debug
#include <debug.h>

const byte PLUG_COUNT = 7;
const byte PLUG_INVALID = 255;

Panel::Panel()
{
    m_plugs = NULL;
};

bool Panel::init(bool is_sn_last_digit_odd)
{
    m_sn_last_digit_odd = is_sn_last_digit_odd;
    if (m_plugs == NULL)
    {
        m_plugs = (struct Plug *)malloc(PLUG_COUNT * sizeof(Plug));
    }
    // initialise all wires
    for (byte x = 0; x < PLUG_COUNT; x++)
    {
        m_plugs[x] = Plug(6-x); // because i installed the field incorrectly by 180°
    }

    byte wirecount = 0;
    for (byte x = 0; x < PLUG_COUNT; x++)
    {
        if (m_plugs[x].hasWire())
        {
            m_plugs[x].m_number = wirecount;
            wirecount++;
        }
    }

    // checking if there are enough wires installed
    if (wirecount < 3)
    {
        dbgOut("wrong wire count: ");
        dbgOutLn2(wirecount, DEC);
        return false;
    }

    m_defusePlug = PLUG_INVALID;

    // evaluating the plug to unplug
    switch (wirecount)
    {
    case 3:
        m_defusePlug = get3WireDefusePlug();
        Serial.print(F("3 wire "));
        break;
    case 4:
        m_defusePlug = get4WireDefusePlug();
        Serial.print(F("4 wire "));
        break;
    case 5:
        m_defusePlug = get5WireDefusePlug();
        Serial.print(F("5 wire "));
        break;
    case 6:
        m_defusePlug = get6WireDefusePlug();
        Serial.print(F("6 wire "));
        break;
    };
    Serial.print(F("defuse plug: "));
    Serial.println(m_defusePlug);
    return true;
}

byte Panel::get3WireDefusePlug()
{
    if (countColor(WIRECOLORS::RED) == 0)
    {
        return getPlug(2);
    }
    if (isLastColor(WIRECOLORS::WHITE))
    {
        return getLastPlug();
    }
    if (countColor(WIRECOLORS::BLUE) > 1)
    {
        return getLastPlugOfColor(WIRECOLORS::BLUE);
    }
    return getLastPlug();
}

byte Panel::get4WireDefusePlug()
{
    if ((countColor(WIRECOLORS::RED) > 1) && m_sn_last_digit_odd)
    {
        return getLastPlugOfColor(WIRECOLORS::RED);
    }
    if ((isLastColor(WIRECOLORS::YELLOW)) && (countColor(WIRECOLORS::RED) == 0))
    {
        return getPlug(1);
    }
    if (countColor(WIRECOLORS::BLUE) == 1)
    {
        return getPlug(1);
    }
    if (countColor(WIRECOLORS::YELLOW) > 1)
    {
        return getLastPlug();
    }
    return getPlug(2);
}

byte Panel::get5WireDefusePlug()
{
    if ((isLastColor(WIRECOLORS::BLACK) && m_sn_last_digit_odd))
    {
        return getPlug(4);
    }
    if ((countColor(WIRECOLORS::RED) == 1) && countColor(WIRECOLORS::YELLOW) > 1)
    {
        return getPlug(1);
    }
    if (countColor(WIRECOLORS::BLACK) == 0)
    {
        return getPlug(2);
    }
    return getPlug(1);
}

byte Panel::get6WireDefusePlug()
{
    if ((countColor(WIRECOLORS::YELLOW) == 0) && m_sn_last_digit_odd)
    {
        return getPlug(3);
    }
    if ((countColor(WIRECOLORS::YELLOW) == 1) && (countColor(WIRECOLORS::WHITE) > 0))
    {
        return getPlug(4);
    }
    if (countColor(WIRECOLORS::RED) == 0)
    {
        return getLastPlug();
    }
    return getPlug(4);
}

byte Panel::getPlug(byte position)
{
    byte count = 0;
    for (byte x = 0; x < PLUG_COUNT; x++)
    {
        if (m_plugs[x].hasWire())
        {
            count++;
            if (count == position)
            {
                return x;
            }
        }
    }
    return PLUG_INVALID;
}

byte Panel::getLastPlug()
{
    for (byte x = PLUG_COUNT; x > 0; x--)
    {
        if (m_plugs[x - 1].hasWire())
        {
            return x - 1;
        }
    }
    return PLUG_INVALID;
}

byte Panel::getLastPlugOfColor(WIRECOLORS color)
{
    for (byte x = PLUG_COUNT; x > 0; x--)
    {
        if (m_plugs[x - 1].hasWire() && (m_plugs[x - 1].initial().color == color))
        {
            return x - 1;
        }
    }
    return PLUG_INVALID;
}

bool Panel::isLastColor(WIRECOLORS color)
{
    byte pidx = getLastPlug();
    if (pidx == PLUG_INVALID)
    {
        return false;
    }
    return m_plugs[pidx].actual().color == color;
}

byte Panel::countColor(WIRECOLORS color)
{
    byte count = 0;
    for (byte x = 0; x < PLUG_COUNT; x++)
    {
        if (m_plugs[x].initial().color == color)
        {
            count++;
        }
    }
    return count;
}

bool Panel::isDisarmed()
{
    if ((m_defusePlug < PLUG_INVALID) && !m_plugs[m_defusePlug].actualHasWire())
    {
        return true;
    }
    return false;
}

bool Panel::isStriken()
{
    for(byte x = 0; x < PLUG_COUNT; x++) {
        if (m_plugs[x].hasWire() && x != m_defusePlug) {
            if (!m_plugs[x].actualHasWire()) {
                return true;
            }
        }
    }
    return false;
}

void Panel::printPlugs() {
    for (byte x = 0; x < PLUG_COUNT; x++)
    {
        Serial.print(x);
        m_plugs[x].hasWire() ? Serial.print("* ") : Serial.print("  ");
        Serial.print(WireNames[m_plugs[x].actual().color]);
        Serial.println();
    }
}
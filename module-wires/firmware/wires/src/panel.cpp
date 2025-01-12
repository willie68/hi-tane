// Implementation of the patch panel
#include <Arduino.h>
#include <panel.h>

const byte PLUG_COUNT = 6;
const byte PLUG_INVALID = 255;

Panel::Panel()
{
    plugs = NULL;
};

bool Panel::init(bool is_sn_last_digit_odd)
{
    sn_last_digit_odd = is_sn_last_digit_odd;
    if (plugs == NULL)
    {
        plugs = (struct Plug *)malloc(PLUG_COUNT * sizeof(Plug));
    }
    // initialise all wires
    for (byte x = 0; x < PLUG_COUNT; x++)
    {
        plugs[x] = Plug(x);
    }

    byte wirecount = 0;
    for (byte x = 0; x < PLUG_COUNT; x++)
    {
        if (plugs[x].hasWire())
        {
            plugs[x].number = wirecount;
            wirecount++;
        }
    }

    // checking if there are enough wires installed
    if (wirecount < 3)
    {
        return false;
    }

    defusePlug = PLUG_INVALID;

    // evaluating the plug to unplug
    switch (wirecount)
    {
    case 3:
        defusePlug = get3WireDefusePlug();
        break;
    case 4:
        defusePlug = get4WireDefusePlug();
        break;
    case 5:
        defusePlug = get5WireDefusePlug();
        break;
    case 6:
        defusePlug = get6WireDefusePlug();
        break;
    };

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
    if ((countColor(WIRECOLORS::RED) > 1) && sn_last_digit_odd)
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
    if ((isLastColor(WIRECOLORS::BLACK) && sn_last_digit_odd))
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
    if ((countColor(WIRECOLORS::YELLOW) == 0) && sn_last_digit_odd)
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
        if (plugs[x].hasWire())
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
        if (plugs[x - 1].hasWire())
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
        if (plugs[x - 1].hasWire() && (plugs[x - 1].initial().color == color))
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
    return plugs[pidx].actual().color == color;
}

byte Panel::countColor(WIRECOLORS color)
{
    byte count = 0;
    for (byte x = 0; x < PLUG_COUNT; x++)
    {
        if (plugs[x].initial().color == color)
        {
            count++;
        }
    }
    return count;
}

bool Panel::isDisarmed()
{
    if ((defusePlug < PLUG_INVALID) && plugs[defusePlug].actualHasWire())
    {
        return true;
    }
    return false;
}

bool Panel::isStriken()
{
    for(byte x = 0; x < PLUG_COUNT; x++) {
        if (plugs[x].hasWire() && x != defusePlug) {
            if (!plugs[x].actualHasWire()) {
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
        plugs[x].hasWire() ? Serial.print("* ") : Serial.print("  ");
        Serial.print(WireNames[plugs[x].actual().color]);
        Serial.println();
    }

}
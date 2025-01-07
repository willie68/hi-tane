// Implementation of the patch panel
#include <Arduino.h>
#include <panel.h>

const byte PLUG_COUNT = 6;
const byte PLUG_INVALID = 255;

Panel::Panel()
{
    plugs = NULL;
};

bool Panel::init()
{
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

    thePlug = PLUG_INVALID;

    // evaluating the plug to unplug
    switch (wirecount)
    {
    case 3:
        if (countColor(WIRECOLORS::RED) == 0)
        {
            thePlug = getPlug(2);
        }
        else if (isLastColor(WIRECOLORS::WHITE))
        {
            thePlug = getLastPlug();
        }
        else if (countColor(WIRECOLORS::BLUE) > 1)
        {
            thePlug = getLastPlugOfColor(WIRECOLORS::BLUE);
        };
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:
        break;
    };

    return true;
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
        if (plugs[x-1].hasWire())
        {
            return x-1;
        }
    }
    return PLUG_INVALID;
}

byte Panel::getLastPlugOfColor(WIRECOLORS color) {
    for (byte x = PLUG_COUNT; x > 0; x--)
    {
        if (plugs[x-1].hasWire() && (plugs[x-1].initial().color == color))
        {
            return x-1;
        }
    }
    return PLUG_INVALID;
}

bool Panel::isLastColor(WIRECOLORS color) {
    byte pidx = getLastPlug();
    if (pidx == PLUG_INVALID) {
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

bool Panel::isUnarmed()
{
    if ((thePlug < PLUG_INVALID) && plugs[thePlug].actualHasWire())
    {
        return true;
    }
    return false;
}
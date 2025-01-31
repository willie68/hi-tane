// How can i calculate the color of a wire?
#include "wire.h"

Wire getWireFromADV(int value) {
    for (byte x = 0; x < WIRE_COLOR_COUNT; x++) {
        if ((value > Wires[x].min) && (value < Wires[x].max)) {
            return  Wires[x];
        }
    }    
    return WIRE_NONE;
}


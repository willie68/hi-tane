// What is a wire?
#ifndef WIRE_H
#define WIRE_H

#include <Arduino.h>

enum WIRECOLORS
{
    NONE = 0,
    RED,
    WHITE,
    BLUE,
    YELLOW,
    BLACK,
    GREEN,
    BROWN
};

struct Wire
{
    WIRECOLORS color;
    int min;
    int max;
};

const byte WIRE_COLOR_COUNT = 8;

const Wire WIRE_NONE = {NONE, 0, 20};
const Wire WIRE_RED = {RED, 900, 960};
const Wire WIRE_WHITE = {WHITE, 760, 810};
const Wire WIRE_BLUE = {BLUE, 590, 620};
const Wire WIRE_YELLOW = {YELLOW, 430, 490};
const Wire WIRE_BLACK = {BLACK, 300, 360};
const Wire WIRE_GREEN = {GREEN, 150, 180};
const Wire WIRE_BROWN = {BROWN, 30, 60};

const Wire Wires[WIRE_COLOR_COUNT] = {
    WIRE_NONE, WIRE_RED, WIRE_WHITE, WIRE_BLUE, WIRE_YELLOW, WIRE_BLACK, WIRE_GREEN, WIRE_BROWN};

const String WireNames[WIRE_COLOR_COUNT] = {
    "nn", "red", "white", "blue", "yellow", "black", "green", "brown"};

Wire getWireFromADV(int value);

#endif
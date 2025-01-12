#include <Arduino.h>
#include <mazes.h>

bool Maze::init() {
    field = mazedefs[3];
    return true;
}

bool Maze::isDisarmed()
{
    return false;
}

bool Maze::isStriken()
{
    return false;
}
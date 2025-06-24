#ifndef DIFFICULTY_H
#define DIFFICULTY_H

#include <Arduino.h>

enum Difficulty
{
    SIMPLE = 0,
    MEDIUM = 1,
    HARD = 2,
    NUM_DIFF = 3
};

void nextDiff(Difficulty &diff);
void prevDiff(Difficulty &diff);

#endif
#include <difficulty.h>

void nextDiff(Difficulty &diff)
{
    int idx = static_cast<int>(diff);
    idx++;
    if (idx == NUM_DIFF)
    {
        idx--;
    }
    diff = static_cast<Difficulty>(idx);
}

void prevDiff(Difficulty &diff)
{
    int idx = static_cast<int>(diff);
    idx--;
    if (idx < 0)
    {
        idx = 0;
    }
    diff = static_cast<Difficulty>(idx);
}

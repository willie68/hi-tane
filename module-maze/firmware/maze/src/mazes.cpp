#include <Arduino.h>
#include <mazes.h>

void MazeField::load(Difficulty difficulty, byte idx)
{
    hard = false;
    dim = 6;
    if (difficulty != Difficulty::HARD)
    {
        memcpy_P(&field6, &(mazedefs[idx]), sizeof(MazeDef6x6));
    }
    else
    {
        memcpy_P(&field8, &(mazedefs8[idx]), sizeof(MazeDef8x8));
        dim = 8;
        hard = true;
    }
};

MarkerT MazeField::getMarker()
{
    if (hard)
    {
        return field8.marker;
    }
    return field6.marker;
};

byte MazeField::getValue(byte x, byte y)
{
    if (hard)
    {
        return field8.maze[y][x];
    }
    return field6.maze[y][x];
};

byte MazeField::getDim()
{
    return dim;
};

Maze::Maze()
{
}

bool Maze::init(Difficulty difficulty)
{
    this->difficulty = difficulty;
    randomSeed(analogRead(0));
    byte idx = random(DEF_COUNT);
    field = MazeField();
    field.load(difficulty, idx);

    byte distance = 0;
    byte minDist = 8;
    byte maxDist = 10;
    if (difficulty == Difficulty::MEDIUM)
    {
        minDist = 10;
        maxDist = 12;
    }
    else if (difficulty == Difficulty::HARD)
    {
        minDist = 10;
        maxDist = 40;
    }
    while ((distance >= maxDist) || (distance < minDist))
    {
        player.x = random(0, field.getDim());
        player.y = random(0, field.getDim());
        goal.x = random(0, field.getDim());
        goal.y = random(0, field.getDim());
        distance = getDistance();
    }
    return true;
}

bool Maze::isSolved()
{
    return ((player.x == goal.x) && (player.y == goal.y));
}

byte Maze::pos2index(Position p)
{
    return pos2index(p.x, p.y);
}

byte Maze::pos2index(byte x, byte y)
{
    if (difficulty == Difficulty::HARD)
    {
        return x + (y * 8);
    }
    return 9 + x + (y * 8);
}

MarkerT Maze::getMarker()
{
    MarkerT mark;
    for (byte x = 0; x < 2; x++)
    {
        mark.marker[x] = pos2index(((field.getMarker().marker[x] & 0xF0) >> 4), (field.getMarker().marker[x] & 0x0F));
    }
    return mark;
}

byte Maze::getPlayer()
{
    return pos2index(player);
}

byte Maze::getGoal()
{
    return pos2index(goal);
}

byte Maze::getDistance()
{

    byte x = player.x;
    byte y = player.y;
    byte dist = 0;

    while (!recurDist(x, y, 0, &dist))
        ;

    return dist;
}

bool Maze::recurDist(byte x, byte y, byte dir, byte *dist)
{
    if ((x == goal.x) && (y == goal.y))
    {
        return true;
    }
    bool found = false;
    byte value = field.getValue(x, y);
    if (((value & N) == 0) && (dir != S))
    {
        found = recurDist(x, y - 1, N, dist);
        if (found)
        {
            (*dist)++;
            return true;
        }
    }
    if (((value & E) == 0) && (dir != W))
    {
        found = recurDist(x + 1, y, E, dist);
        if (found)
        {
            (*dist)++;
            return true;
        }
    }
    if (((value & S) == 0) && (dir != N))
    {
        found = recurDist(x, y + 1, S, dist);
        if (found)
        {
            (*dist)++;
            return true;
        }
    }
    if (((value & W) == 0) && (dir != E))
    {
        found = recurDist(x - 1, y, W, dist);
        if (found)
        {
            (*dist)++;
            return true;
        }
    }
    return false;
}

bool Maze::plN()
{
    if ((player.y > 0) && (field.getValue(player.x, player.y) & N) == 0)
    {
        player.y--;
        return false;
    }
    else
    {
        return true;
    }
};

bool Maze::plE()
{
    if ((player.x < (field.getDim()-1)) && (field.getValue(player.x, player.y) & E) == 0)
    {
        player.x++;
        return false;
    }
    else
    {
        return true;
    }
};

bool Maze::plS()
{
    if ((player.y < (field.getDim()-1)) && (field.getValue(player.x, player.y) & S) == 0)
    {
        player.y++;
        return false;
    }
    else
    {
        return true;
    }
};

bool Maze::plW()
{
    if ((player.x > 0) && (field.getValue(player.x, player.y) & W) == 0)
    {
        player.x--;
        return false;
    }
    else
    {
        return true;
    }
};

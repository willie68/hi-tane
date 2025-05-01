#include <Arduino.h>
#include <mazes.h>
#define debug
#include <debug.h>

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
    this->m_difficulty = difficulty;
    randomSeed(analogRead(0));
    byte idx = random(DEF_COUNT);
    m_field = MazeField();
    m_field.load(difficulty, idx);

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
        m_player.x = random(0, m_field.getDim());
        m_player.y = random(0, m_field.getDim());
        m_goal.x = random(0, m_field.getDim());
        m_goal.y = random(0, m_field.getDim());
        distance = getDistance();
    }
    dbgOut(F("maze:"));
    dbgOut(difficulty);
    dbgOut(":");
    dbgOutLn(idx);
    dbgOut(F("player:"));
    dbgOut(m_player.x);
    dbgOut(":");
    dbgOutLn(m_player.y);
    dbgOut(F("goal:"));
    dbgOut(m_goal.x);
    dbgOut(":");
    dbgOutLn(m_goal.y);
    dbgOut(F("dist:"));
    dbgOutLn(distance);
    return true;
}

bool Maze::isSolved()
{
    return ((m_player.x == m_goal.x) && (m_player.y == m_goal.y));
}

byte Maze::pos2index(Position p)
{
    return pos2index(p.x, p.y, false);
}

byte Maze::pos2index(byte x, byte y,bool direct)
{
    if ((m_difficulty == Difficulty::HARD) || direct)
    {
        return (7 - x) + (y * 8);
    }
    return 9 + (5 - x) + (y * 8);
}

MarkerT Maze::getMarker()
{
    MarkerT mark;
    for (byte x = 0; x < 2; x++)
    {
        mark.marker[x] = pos2index(((m_field.getMarker().marker[x] & 0xF0) >> 4), (m_field.getMarker().marker[x] & 0x0F), false);
    }
    return mark;
}

byte Maze::getPlayer()
{
    return pos2index(m_player);
}

byte Maze::getGoal()
{
    return pos2index(m_goal);
}

byte Maze::getDistance()
{

    byte x = m_player.x;
    byte y = m_player.y;
    byte dist = 0;

    while (!recurDist(x, y, 0, &dist))
        ;

    return dist;
}

bool Maze::recurDist(byte x, byte y, byte dir, byte *dist)
{
    if ((x == m_goal.x) && (y == m_goal.y))
    {
        return true;
    }
    bool found = false;
    byte value = m_field.getValue(x, y);
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
    if ((m_player.y > 0) && (m_field.getValue(m_player.x, m_player.y) & N) == 0)
    {
        m_player.y--;
        printPlayer();
        return false;
    }
    else
    {
        return true;
    }
};

bool Maze::plE()
{
    if ((m_player.x < (m_field.getDim() - 1)) && (m_field.getValue(m_player.x, m_player.y) & E) == 0)
    {
        m_player.x++;
        printPlayer();
        return false;
    }
    else
    {
        return true;
    }
};

bool Maze::plS()
{
    if ((m_player.y < (m_field.getDim() - 1)) && (m_field.getValue(m_player.x, m_player.y) & S) == 0)
    {
        m_player.y++;
        printPlayer();
        return false;
    }
    else
    {
        return true;
    }
};

bool Maze::plW()
{
    if ((m_player.x > 0) && (m_field.getValue(m_player.x, m_player.y) & W) == 0)
    {
        m_player.x--;
        printPlayer();
        return false;
    }
    else
    {
        return true;
    }
};

void Maze::printPlayer()
{
    Serial.print("ply: ");
    Serial.print(m_player.x);
    Serial.print(" / ");
    Serial.print(m_player.y);
    Serial.print(" = ");
    Serial.println(pos2index(m_player.x, m_player.y, false));
}
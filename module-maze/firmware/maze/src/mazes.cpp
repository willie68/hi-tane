#include <Arduino.h>
#include <mazes.h>

bool Maze::init()
{
    randomSeed(analogRead(0));
    byte idx = random(DEF_COUNT);
    field = mazedefs[idx];
    Serial.print("b:");
    Serial.print(idx);
    byte distance = 0;
    while (distance < 10)
    {
        player.x = random(0, 5);
        player.y = random(0, 5);
        goal.x = random(0, 5);
        goal.y = random(0, 5);
        Serial.print("p.x: ");
        Serial.print(player.x);
        Serial.print(", p.y: ");
        Serial.print(player.y);
        Serial.print(", g.x: ");
        Serial.print(goal.x);
        Serial.print(", g.y: ");
        Serial.println(goal.y);
        distance = getDistance();
        Serial.print("dist: ");
        Serial.println(distance);
        delay(1000);
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
    return x + y * 6;
}

MarkerT Maze::getMarker()
{
    MarkerT mark;
    for (byte x = 0; x < 2; x++)
    {
        mark.marker[x] = pos2index(((field.marker.marker[x] & 0xF0) >> 4), (field.marker.marker[x] & 0x0F));
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
#ifdef debug
    Serial.print("x: ");
    Serial.print(x);
    Serial.print(", y: ");
    Serial.print(y);
    Serial.println();
#endif
    if ((x == goal.x) && (y == goal.y))
    {
#ifdef debug
        Serial.println(" goal");
#endif
        return true;
    }
    bool found = false;
    if (((field.maze[y][x] & N) == 0) && (dir != S))
    {
        found = recurDist(x, y - 1, N, dist);
        if (found)
        {
            *dist += 1;
            return true;
        }
    }
    if (((field.maze[y][x] & E) == 0) && (dir != W))
    {
        found = recurDist(x + 1, y, E, dist);
        if (found)
        {
            *dist += 1;
            return true;
        }
    }
    if (((field.maze[y][x] & S) == 0) && (dir != N))
    {
        found = recurDist(x, y + 1, S, dist);
        if (found)
        {
            *dist += 1;
            return true;
        }
    }
    if (((field.maze[y][x] & W) == 0) && (dir != E))
    {
        found = recurDist(x - 1, y, W, dist);
        if (found)
        {
            *dist += 1;
            return true;
        }
    }
    return false;
}

bool Maze::plN()
{
    if ((player.y > 0) && (field.maze[player.y][player.x] & N) == 0)
    {
        player.y -= 1;
        return false;
    }
    else
    {
        return true;
    }
};
bool Maze::plE()
{
    if ((player.x < 5) && (field.maze[player.y][player.x] & E) == 0)
    {
        player.x += 1;
        return false;
    }
    else
    {
        return true;
    }
};
bool Maze::plS()
{
    if ((player.y < 5) && (field.maze[player.y][player.x] & S) == 0)
    {
        player.y += 1;
        return false;
    }
    else
    {
        return true;
    }
};
bool Maze::plW()
{
    if ((player.x > 0) && (field.maze[player.y][player.x] & W) == 0)
    {
        player.x -= 1;
        return false;
    }
    else
    {
        return true;
    }
};

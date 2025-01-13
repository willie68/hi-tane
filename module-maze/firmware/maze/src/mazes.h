// Definition of the patch panel component
#ifndef MAZES_H
#define MAZES_H

#include <Arduino.h>
enum DIRECTIONS
{
  N = 0x01,
  E = 0x02,
  S = 0x04,
  W = 0x08
};

#define NE N | E
#define NS N | S
#define NW N | W
#define EW E | W
#define ES E | S
#define SW S | W
#define NES N | E | S
#define NSW N | S | W
#define NEW N | E | W
#define ESW E | S | W

struct MarkerT
{
  byte marker[2];
};

struct Position
{
  byte x, y;
};

struct MazeDef
{
  byte maze[6][6];
  MarkerT marker;
};

const byte DEF_COUNT = 9;

const MazeDef mazedefs[DEF_COUNT] = {
    {{{NW, NS, NE, NW, NS, NES},
      {EW, NW, ES, SW, NS, NE},
      {EW, SW, NE, NW, NS, E},
      {EW, NSW, S, ES, NSW, E},
      {W, NS, NE, NW, NES, EW},
      {SW, NES, SW, ES, NSW, ES}},
     {0x01, 0x52}},
    {{{NSW, N, NES, NW, N, NES},
      {NW, ES, NW, ES, SW, NE},
      {EW, NW, ES, NW, NS, E},
      {W, ES, NW, ES, NEW, EW},
      {EW, NEW, EW, NW, ES, EW},
      {ESW, SW, ES, SW, NS, ES}},
     {0x13, 0x41}},
    {{{NW, NS, NE, NEW, NW, NE},
      {ESW, NEW, EW, SW, ES, EW},
      {NW, E, EW, NW, NE, EW},
      {EW, EW, EW, EW, EW, EW},
      {EW, SW, ES, EW, EW, EW},
      {SW, NS, NS, ES, SW, ES}},
     {0x33, 0x53}},
    {{{NW, NE, NSW, NS, NS, NE},
      {EW, EW, NW, NS, NS, E},
      {EW, SW, ES, NW, NES, EW},
      {EW, NSW, NS, S, NS, E},
      {W, NS, NS, NS, NE, EW},
      {SW, NS, NES, NSW, ES, ESW}},
     {0x00, 0x03}},
    {{{NSW, NS, NS, NS, N, NE},
      {NW, NS, NS, N, ES, ESW},
      {W, NE, NSW, S, NW, NE},
      {EW, SW, NS, N, ESW, EW},
      {EW, NW, NS, S, NEW, EW},
      {ESW, SW, NS, NS, NS, ES}},
     {0x42, 0x35}},
    {{{NEW, NW, NE, NSW, N, NE},
      {EW, EW, EW, NW, ES, EW},
      {W, ES, ESW, EW, NW, ES},
      {SW, NE, NW, E, EW, NEW},
      {NW, ES, ESW, EW, SW, E},
      {SW, NS, NS, S, NSW, ES}},
     {0x40, 0x24}},
    {{{NW, NS, NS, NE, NW, NE},
      {EW, NW, NE, SW, ES, EW},
      {SW, ES, NW, NES, NW, ES},
      {NW, NE, W, NS, ES, NEW},
      {EW, ESW, SW, NS, NE, EW},
      {SW, NS, NS, NS, S, ES}},
     {0x10, 0x15}},
    {{{NEW, NW, NS, NE, NW, NE},
      {W, S, NES, SW, ES, EW},
      {EW, NW, NS, NS, NE, EW},
      {EW, SW, NE, NSW, S, ES},
      {EW, NEW, SW, NS, NS, NES},
      {SW, S, NS, NS, NS, NES}},
     {0x30, 0x23}},
    {{{NEW, NW, NS, NS, N, NE},
      {EW, EW, NW, NES, EW, EW},
      {W, S, ES, NW, ES, EW},
      {EW, NEW, NW, ES, NSW, E},
      {EW, EW, EW, NW, NE, ESW},
      {SW, ES, SW, ES, SW, NES}},
     {0x21, 0x05}}};

struct Maze
{
public:
  bool init();

  bool isDisarmed();
  bool isStriken();
  MarkerT getMarker();
  byte getPlayer();
  byte getGoal();

private:
  MazeDef field;
  byte startPos;
  byte endPos;

  Position player, goal;

  byte getDistance();

  bool recurDist(byte x, byte y, byte dir, byte *dist);

  byte pos2index(Position p);
  byte pos2index(byte x, byte y);
};

#endif
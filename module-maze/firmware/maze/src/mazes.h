// Definition of the patch panel component
#ifndef MAZES_H
#define MAZES_H

#include <Arduino.h>
#include <game.h>

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

struct MazeDef6x6
{
  byte maze[6][6];
  MarkerT marker;
};

struct MazeDef8x8
{
  byte maze[8][8];
  MarkerT marker;
};

const byte DEF_COUNT = 9;

const MazeDef6x6 mazedefs[DEF_COUNT] PROGMEM = {
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
      {SW, NS, NS, ES, NSW, ES}},
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
     {0x21, 0x04}}};

const MazeDef8x8 mazedefs8[DEF_COUNT] PROGMEM = {
    {{{NW, NS, NES, N, NS, NS, NS, NE},
      {W, N, NS, E, NW, NS, NES, EW},
      {EW, EW, NW, ES, SW, NS, N, E},
      {EW, EW, SW, NE, NW, NS, E, EW},
      {ESW, EW, NSW, S, ES, NSW, E, EW},
      {NEW, W, NS, NE, NW, NES, EW, ESW},
      {EW, SW, NES, SW, ES, NSW, ES, NEW},
      {SW, NS, S, NES, NSW, S, NS, ES}},
     {0x12, 0x63}},
    {{{NW, NS, NS, NES, NSW, NS, NS, NE},
      {W, NS, N, NES, NW, N, NES, EW},
      {EW, NW, ES, NW, ES, SW, NE, EW},
      {EW, EW, NW, ES, NW, NS, 0, E},
      {EW, W, ES, NW, ES, NEW, EW, EW},
      {EW, EW, NEW, EW, NW, ES, EW, EW},
      {W, ES, SW, ES, W, NS, ES, ESW},
      {ESW, SW, NS, NS, N, NS, NS, NES}},
     {0x24, 0x52}},
    {{{NW, NS, NS, NS, NS, NE, NSW, NE},
      {W, N, NS, NE, NEW, NW, NE, EW},
      {EW, ESW, NEW, EW, SW, ES, EW, EW},
      {ESW, NW, E, EW, NW, NE, EW, EW},
      {NEW, EW, EW, EW, EW, EW, EW, EW},
      {EW, EW, SW, ES, EW, EW, W, E},
      {EW, S, NS, NS, ES, SW, ES, EW},
      {SW, S, NS, NS, NEW, NSW, NS, ES}},
     {0x44, 0x64}},
    {{{NW, NS, NS, NS, NE, NSW, N, NE},
      {EW, NW, NE, NSW, S, NS, E, EW},
      {EW, EW, EW, NW, NS, NS, E, EW},
      {ESW, EW, SW, ES, NW, NES, EW, EW},
      {NEW, EW, NSW, NS, S, NS, E, EW},
      {EW, W, NS, NS, NS, NE, EW, EW},
      {EW, SW, NS, NE, NSW, ES, EW, EW},
      {SW, NS, NS, S, NES, NSW, S, ES}},
     {0x11, 0x14}},
    {{{NW, NS, NES, NSW, NS, N, NS, NE},
      {EW, NSW, NS, NS, NS, N, NE, EW},
      {W, N, NS, NS, N, ES, ESW, ESW},
      {EW, W, NE, NSW, S, NW, NE, NEW},
      {EW, EW, SW, NS, N, ESW, EW, EW},
      {EW, EW, NW, NS, S, NEW, W, E},
      {EW, EW, SW, NS, NS, NS, ES, EW},
      {ESW, ESW, NSW, NS, NS, NS, NS, ES}},
     {0x53, 0x46}},
    {{{NW, NS, N, NS, NS, NS, NS, NE},
      {ESW, NEW, W, NE, NSW, N, NE, ESW},
      {NEW, EW, EW, EW, NW, ES, EW, NEW},
      {EW, W, ES, ESW, EW, NW, ES, EW},
      {EW, SW, NE, NW, E, EW, NEW, EW},
      {W, N, ES, ESW, EW, SW, 0, E},
      {EW, SW, NS, NS, E, NSW, ES, EW},
      {SW, NS, NES, NSW, S, NS, NS, ES}},
     {0x51, 0x35}},
    {{{NW, NS, NS, NE, NSW, NS, NS, NE},
      {EW, NW, S, NS, NE, NW, N, ES},
      {EW, EW, NW, NE, SW, ES, EW, NEW},
      {EW, SW, ES, NW, NES, NW, ES, EW},
      {ESW, NW, NE, W, NS, ES, NEW, EW},
      {W, EW, ESW, SW, NS, NE, W, E},
      {EW, SW, NS, NS, NS, S, ES, EW},
      {SW, NS, NS, NS, NS, NS, NS, ES}},
     {0x21, 0x26}},
    {{{NEW, NEW, NSW, NS, NS, NS, N, NE},
      {EW, EW, NW, NS, NE, NW, E, NW},
      {EW, W, S, NES, SW, ES, EW, EW},
      {SW, E, NW, NS, NS, NE, EW, EW},
      {NEW, EW, SW, NE, NSW, S, ES, ESW},
      {EW, EW, NEW, SW, NS, NS, NS, NES},
      {EW, SW, S, NS, N, NS, NES, NEW},
      {SW, NS, NS, NS, S, NS, NS, ES}},
     {0x41, 0x34}},
    {{{NW, NE, NS, NS, NS, NS, N, NE},
      {EW, EW, NW, NS, NS, N, E, EW},
      {EW, EW, EW, NW, NES, EW, EW, EW},
      {EW, W, S, ES, NW, ES, EW, EW},
      {EW, EW, NEW, NW, ES, NSW, E, ESW},
      {ESW, EW, EW, EW, NW, NE, ESW, NEW},
      {NEW, SW, E, SW, ES, SW, NS, E},
      {SW, NS, S, NS, NS, NES, NSW, ES}},
     {0x32, 0x15}}};

class MazeField
{
public:
  void load(Difficulty difficulty, byte idx);
  MarkerT getMarker();
  byte getValue(byte x, byte y);
  byte getDim();

private:
  MazeDef6x6 field6;
  MazeDef8x8 field8;
  byte dim;
  bool hard;
};

struct Maze
{
public:
  Maze();
  bool init(Difficulty difficulty);

  bool isSolved();
  MarkerT getMarker();
  byte getPlayer();
  byte getGoal();

  bool plN();
  bool plE();
  bool plS();
  bool plW();

private:
  MazeField field;
  byte startPos;
  byte endPos;

  Position player, goal;

  Difficulty difficulty;
  byte getDistance();

  bool recurDist(byte x, byte y, byte dir, byte *dist);

  byte pos2index(Position p);
  byte pos2index(byte x, byte y);
};

#endif
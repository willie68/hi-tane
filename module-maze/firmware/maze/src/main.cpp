#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <avdweb_Switch.h>

#define debug
#define wokwi
#include "indicators.h"
#include <mazes.h>
#include "game.h"

// ---- forward declarations
void initGame();
void btnpoll();
void showBoard(bool smo);
void showSmile(bool ok);

// RGB LED
#define LED_PIN 4
#define COM_PIN 11
// Game framework
Game game(ModuleTag::MAZE, LED_PIN, COM_PIN);

#define MATRIX_PIN 5
const byte MATRIX_LED_COUNT = 64;
Adafruit_NeoPixel matrix(MATRIX_LED_COUNT, MATRIX_PIN, NEO_GRB + NEO_KHZ800);

Switch btn = Switch(6);  // Button north
Switch bte = Switch(7);  // Button east
Switch bts = Switch(8);  // Button south
Switch btw = Switch(9);  // Button west
Switch btm = Switch(10); // Button middle

Maze maze;

void setup()
{
  Serial.begin(115200);

  game.setState(ModuleState::INIT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MATRIX_PIN, OUTPUT);
  matrix.setBrightness(64);
  initGame();

  game.arm();
  matrix.setBrightness(0x20);
  matrix.show();
}

unsigned long showMarkerOnly = 0;
bool strike = false;

void loop()
{
  game.poll();
  btnpoll();

  bool strike = false;
  if (btn.singleClick())
    strike = maze.plN();
  if (bte.singleClick())
    strike = maze.plE();
  if (bts.singleClick())
    strike = maze.plS();
  if (btw.singleClick())
    strike = maze.plW();
  if (btm.singleClick())
    showMarkerOnly = millis() + 5000;

  if (maze.isSolved() && game.isState(ModuleState::ARMED))
  {
    game.setSolved();
    showSmile(true);
    for (byte x = 100; x > 0; x--)
    {
      game.poll();
      delay(10);
    }
  }
  else if (strike && game.isState(ModuleState::ARMED))
  {
    game.setStrike();
    showSmile(false);
    for (byte x = 100; x > 0; x--)
    {
      game.poll();
      delay(10);
    }
  }
  else if (game.isState(ModuleState::STRIKED) && !strike)
  {
    game.setState(ModuleState::ARMED);
  }

  bool showMarks = showMarkerOnly > millis();
  if (!game.isState(ModuleState::DISARMED))
  {
    showBoard(showMarks);
  }

  game.showTime();
}

byte SM_OK[] = {0x3C, 0x42, 0xA5, 0x81, 0xA5, 0x99, 0x42, 0x3C};
byte SM_NOK[] = {0x3C, 0x42, 0xA5, 0x81, 0x99, 0xA5, 0x42, 0x3C};

void showSmile(bool ok)
{
  matrix.clear();
  for (byte y = 0; y < 8; y++)
  {
    for (byte x = 0; x < 8; x++)
    {
      byte value = 0;
      if (ok)
      {
        value = SM_OK[y];
      }
      else
      {
        value = SM_NOK[y];
      }
      if ((value & (1 << x)) > 0)
      {
        if (ok)
        {
          matrix.setPixelColor(maze.pos2index(x, y), PX_GREEN);
        }
        else
        {
          matrix.setPixelColor(maze.pos2index(x, y), PX_RED);
        }
      }
    }
  }
  if (ok) {
    matrix.setBrightness(0x10);
  }
  matrix.show();
}

void showBoard(bool smo)
{
  MarkerT marker = maze.getMarker();
  byte pl = maze.getPlayer();
  byte gl = maze.getGoal();
  for (byte x = 0; x < MATRIX_LED_COUNT; x++)
  {
    matrix.setPixelColor(x, PX_BLACK);
    if (((x < 9) || (x > 54) ||
         (x == 15) || (x == 16) ||
         (x == 23) || (x == 24) ||
         (x == 31) || (x == 32) ||
         (x == 39) || (x == 40) ||
         (x == 47) || (x == 48)) &&
        !game.isGameDifficulty(Difficulty::HARD))
    {
      matrix.setPixelColor(x, PX_LOWWHITE);
    }
    if ((x == marker.marker[0]) || (x == marker.marker[1]))
    {
      matrix.setPixelColor(x, PX_YELLOW);
    }
    if (!smo)
    {
      if (x == pl)
      {
        matrix.setPixelColor(x, PX_WHITE);
      }
      if (x == gl)
      {
        if (maze.isSolved())
          matrix.setPixelColor(x, PX_GREEN);
        else
          matrix.setPixelColor(x, PX_RED);
      }
    }
  }
  matrix.show();
}

void initGame()
{
  game.init();
  game.setGameDifficulty(Difficulty::HARD);

  bool invalid = true;
  while (invalid)
  {
    invalid = !maze.init(game.getGameDifficulty()); // TODO setting the sn from HTCOM
    invalid = false;
    if (invalid)
    {
      delay(1000);
      Serial.println(F("invalid maze configuration"));
      game.sendError("maze: invalid configuration");
    }
  }

  matrix.setBrightness(0x10);
  for (byte x = 0; x < MATRIX_LED_COUNT; x++)
  {
    matrix.setPixelColor(x, PX_YELLOW);
    matrix.show();
    delay(10);
  }
}

void btnpoll()
{
  btn.poll();
  bte.poll();
  btm.poll();
  bts.poll();
  btw.poll();
}
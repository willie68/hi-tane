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
  Serial.println("init");
  game.setState(ModuleState::INIT);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MATRIX_PIN, OUTPUT);

  initGame();
  game.arm();
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
  }
  else if (strike && game.isState(ModuleState::ARMED))
  {
    game.setStrike();
  }
  else if (game.isState(ModuleState::STRIKED) && !strike)
  {
    game.setState(ModuleState::ARMED);
  }

  bool showMarks = showMarkerOnly > millis();
  showBoard(showMarks);
  game.showTime();
}

void showBoard(bool smo)
{
  MarkerT marker = maze.getMarker();
  byte pl = maze.getPlayer();
  byte gl = maze.getGoal();
  for (byte x = 0; x < MATRIX_LED_COUNT; x++)
  {
    matrix.setPixelColor(x, PX_BLACK);
    if ((x < 9) || (x > 54) || (x == 15) || (x == 16) || (x == 23) || (x == 24) || (x == 31) || (x == 32) || (x == 39) || (x == 40) || (x == 47) || (x == 48))
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
  bool invalid = true;
  while (invalid)
  {
    invalid = !maze.init(); // TODO setting the sn from HTCOM
    invalid = false;
    if (invalid)
    {
      delay(1000);
      Serial.println(F("invalid maze configuration"));
      game.sendError("maze: invalid configuration");
    }
  }

  game.init();
  for (byte x = 0; x < MATRIX_LED_COUNT; x++)
  {
    matrix.setPixelColor(x, PX_YELLOW);
    matrix.show();
    delay(10);
  }

  MarkerT marker = maze.getMarker();
  Serial.print("mrk: ");
  Serial.print(marker.marker[0]);
  Serial.print(", ");
  Serial.print(marker.marker[1]);
  Serial.println();

  byte pl = maze.getPlayer();
  byte gl = maze.getGoal();
  for (byte x = 0; x < MATRIX_LED_COUNT; x++)
  {
    delay(10);
    matrix.setPixelColor(x, PX_BLACK);
    if ((x == marker.marker[0]) || (x == marker.marker[1]))
    {
      matrix.setPixelColor(x, PX_YELLOW);
    }
    if (x == pl)
    {
      matrix.setPixelColor(x, PX_WHITE);
    }
    if (x == gl)
    {
      matrix.setPixelColor(x, PX_RED);
    }
    matrix.show();
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
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define debug
#define wokwi
#include "indicators.h"
#include <mazes.h>
#include "game.h"

// ---- forward declarations
void showTime(int act);

// RGB LED
#define LED_PIN 4
#define COM_PIN 11
// Game framework
Game game(ModuleTag::MAZE, LED_PIN, COM_PIN);

#define MATRIX_PIN 5
const byte MATRIX_LED_COUNT = 36;
Adafruit_NeoPixel matrix(MATRIX_LED_COUNT, MATRIX_PIN, NEO_GRB + NEO_KHZ800);

Maze maze;

// --- forward functions
void initGame();

void setup()
{
  Serial.begin(115200);
  Serial.println("init");
  game.setState(ModuleState::INIT);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MATRIX_PIN, OUTPUT);

  initGame();
  game.setState(ModuleState::ARMED);
}

void loop()
{
  game.poll();

  if (maze.isDisarmed() && game.isState(ModuleState::ARMED))
  {
    game.setState(ModuleState::DISARMED);
  }
  else if (maze.isStriken() && game.isState(ModuleState::ARMED))
  {
    game.setState(ModuleState::STRIKED);
  }
  else if (game.isState(ModuleState::STRIKED) && !maze.isStriken())
  {
    game.setState(ModuleState::ARMED);
  }

  showTime(game.getGameTime());
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
    if (x == pl) {
      matrix.setPixelColor(x, PX_WHITE);
    }
    if (x == gl) {
      matrix.setPixelColor(x, PX_RED);
    }
    matrix.show();
  }
}

int saveTime = 0;
void showTime(int act)
{
  if (act != saveTime)
  {
    saveTime = act;
    bool neg = act < 0;
    int t = abs(act);
    byte sec = t % 60;
    byte min = (t - sec) / 60;
    if (neg)
    {
      Serial.print("-");
      if (min <= 0)
        Serial.print("0");
      Serial.print(min);
    }
    else
    {
      if (min <= 0)
        Serial.print("0");
      Serial.print(min);
    }
    Serial.print(":");
    if (sec <= 0)
      Serial.print("0");
    Serial.println(sec);
  }
}
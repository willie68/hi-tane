#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define debug
#include <debug.h>
#include <game.h>
#include "indicators.h"
#include "communication.h"
#include <panel.h>

// RGB LED
#define LED_PIN 4
// Game framework
Game game(ModuleTag::WIRES, LED_PIN);

Panel panel;

// --- forward functions
void initGame();

void setup()
{
  Serial.begin(115200);
  Serial.println("init");
  pinMode(LED_BUILTIN, OUTPUT);
  game.init();

  initGame();
  game.arm();
}

void loop()
{
  game.poll();
  if (game.isNewGameSettings()) {
    initGame();
    game.arm();
  }
  if (game.isState(ModuleState::ARMED))
  {
    if (panel.isDisarmed())
    {
      game.setSolved();
    }
    else if (panel.isStriken())
    {
      dbgOutLn("Strike");
      game.setStrike();
      while (panel.isStriken()) {
        game.poll();
        delay(10);
      }
      byte count = 100;
      while (count > 0) {
        game.poll();
        delay(10);
        count--;
      }
    }
  }
  else if (!panel.isStriken() && game.isState(ModuleState::STRIKED))
  {
    game.setState(ModuleState::ARMED);
  }
}

void initGame()
{
  bool invalid = true;
  unsigned long sact = millis();
  while (invalid)
  {
    game.poll();
    invalid = !panel.init(true); // TODO setting the sn from HTCOM
    if (invalid)
    {
      if (millis() > sact)
      {
        sact = millis() + 5000;
        Serial.println(F("invalid wire configuration"));
        game.sendError(ERRORS::ERR_INVALID_WIRES);
      }
    }
  }
  panel.printPlugs();
}


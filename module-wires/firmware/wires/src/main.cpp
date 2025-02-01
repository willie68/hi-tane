#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define debug
#include <game.h>
#include "indicators.h"
#include "communication.h"
#include <panel.h>

// RGB LED
#define LED_PIN 4
#define COM_PIN 11
// Game framework
Game game(ModuleTag::WIRES, LED_PIN, COM_PIN);

Panel panel;

// --- forward functions
void initGame();

void setup()
{
  Serial.begin(115200);
  Serial.println("init");
  pinMode(LED_BUILTIN, OUTPUT);

  initGame();
  game.arm();
}

void loop()
{
  game.poll();
  if (game.isState(ModuleState::ARMED))
  {
    if (panel.isDisarmed())
    {
      game.setSolved();
    }
    else if (panel.isStriken())
    {
      game.setStrike();
    }
  }
  else if (!panel.isStriken() && game.isState(ModuleState::STRIKED))
  {
    game.setState(ModuleState::ARMED);
  }
  game.showTime();
}

void initGame()
{
  game.init();
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
        game.setIntLED(true);
      }
    }
  }
  game.setIntLED(false);
  panel.printPlugs();
}

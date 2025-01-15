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
  game.setState(ModuleState::INIT);

  initGame();
  game.arm();
}

void loop()
{

  game.poll();
  if (panel.isDisarmed() && game.isState(ModuleState::ARMED))
  {
    game.setSolved();
  }
  else if (panel.isStriken() && game.isState(ModuleState::ARMED))
  {
    game.setStrike();
  }
  else if (!panel.isStriken() && game.isState(ModuleState::STRIKED))
  {
    game.setState(ModuleState::ARMED);
  }

  game.showTime();
}

void initGame()
{
  bool invalid = true;
  while (invalid)
  {
    invalid = !panel.init(true); // TODO setting the sn from HTCOM
    if (invalid)
    {
      delay(1000);
      Serial.println(F("invalid wire configuration"));
      game.sendError("wires: invalid configuration");
    }
  }

  panel.printPlugs();
}


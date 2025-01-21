#include <Arduino.h>
#include <avdweb_Switch.h>

#define debug
#define wokwi
#include "game.h"

// ---- forward declarations
void initGame();

// RGB LED
#define LED_PIN 4
#define COM_PIN 11
// Game framework
Game game(ModuleTag::MORSE, LED_PIN, COM_PIN);

void setup()
{
  Serial.begin(115200);

  initGame();
}

void initGame()
{
  game.init();
  game.arm();
}

void loop()
{
  game.poll();
}

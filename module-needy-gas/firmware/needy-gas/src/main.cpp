#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <shift7sDigit.h>

#define debug
#include <debug.h>
#include <game.h>
#include "indicators.h"
#include "communication.h"

// RGB LED
#define LED_PIN 4
// Game framework
Game game(ModuleTag::WIRES, LED_PIN);

const byte latchPin = 8; // latch pin of the 74HC595
const byte clockPin = 9; // clock pin of the 74HC595
const byte dataPin = 7;  // data pin of the 74HC595

Shift7Segment display = Shift7Segment(2, dataPin, clockPin, latchPin);

// --- forward functions
void initGame();
void poll();
void updateShiftRegister();

void setup()
{
  Serial.begin(115200);
  Serial.println("init");
  pinMode(LED_BUILTIN, OUTPUT);

  game.init();

  initGame();
  game.arm();
}

void initGame()
{
}

void loop()
{
  poll();
  display.clear();
  display.showNumber(-8);
  delay(1000);
  display.showNumber(-4);
  delay(1000);
  display.showDot(0,true);
  display.showNumber(12);
  delay(1000);
  display.showNumber(2);
  delay(1000);
  display.showDot(1,true);
  display.showDot(0,false);
  display.showNumber(199);
  delay(1000);
  // put your main code here, to run repeatedly:
  delay(500);
}

void poll()
{
  game.poll();
  if (game.isNewGameSettings())
  {
    initGame();
    game.arm();
  }
}
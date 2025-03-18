#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <shift7sDigit.h>
#include <avdweb_Switch.h>
#include <U8g2lib.h>

// #define debug
#include <debug.h>
#include <game.h>
#include "indicators.h"
#include "communication.h"

// RGB LED
#define LED_PIN 4
// Game framework
Game game(ModuleTag::WIRES, LED_PIN);

Switch btr = Switch(5); // Button right
Switch btl = Switch(6); // Button left

const byte latchPin = 8; // latch pin of the 74HC595
const byte clockPin = 9; // clock pin of the 74HC595
const byte dataPin = 7;  // data pin of the 74HC595

Shift7Segment display = Shift7Segment(2, dataPin, clockPin, latchPin);

U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* clock=A5*/ 19, /* data=A4*/ 18);

// --- forward functions
void initDisplay();
void initGame();
void poll();
void updateShiftRegister();
void showEffekt(bool solved);
void showNeedy();

enum NeedyState
{
  NS_INIT,
  NS_WAIT,
  NS_USER
};

NeedyState state = NS_INIT;

void setup()
{
  Serial.begin(115200);
  Serial.println("init");
  randomSeed(analogRead(0));
  pinMode(LED_BUILTIN, OUTPUT);

  initDisplay();

  game.init();

  initGame();
  game.arm();
}

void initDisplay()
{
  u8x8.begin();
  // u8x8.setFont(u8x8_font_courB18_2x3_f);
  u8x8.setFont(u8x8_font_8x13_1x2_f);
  u8x8.clearDisplay();
  u8x8.setPowerSave(1);
  delay(1000);
  u8x8.setPowerSave(0);
}

unsigned long act;
unsigned long stimevalue;
bool changed = true;
byte activeButton;
unsigned long waitSec = 10;
const unsigned long userSec = 60;

void initGame()
{
  state = NS_INIT;
  waitSec = random(180, 600);
  #ifdef debug
    waitSec = 10;
  #endif
  dbgOutLn("wait: ");
  dbgOutLn(waitSec);
  act = millis() + (1000 * waitSec);
  state = NS_WAIT;

  u8x8.clearDisplay();
  u8x8.drawString(2, 1, "Nothing to do");
  changed = true;
  game.arm();
}

void loop()
{
  poll();
  if (state == NS_WAIT)
  {
    byte timeValue = (act - millis()) / 1000;
    if (stimevalue != timeValue)
    {
      stimevalue = timeValue;
      dbgOut(F("wait tv: "));
      dbgOutLn(timeValue);
    }
    if (millis() >= act)
    {
      changed = true;
      state = NS_USER;
      act = millis() + (1000L * userSec);
      game.sendBeep();
    }
  }
  if (state == NS_USER)
  {
    byte timeValue = (act - millis()) / 1000;
    if (stimevalue != timeValue)
    {
      stimevalue = timeValue;
      dbgOut(F("user tv: "));
      dbgOutLn(timeValue);
      display.showNumber(timeValue);
      if (timeValue <=10) {
        game.sendBeep();
      }
    }
    showNeedy();
    
    if ((btr.singleClick() && (activeButton == 1)) || (btl.singleClick() && (activeButton == 0)))
    {
      game.setSolved();
    }
    if ((btr.singleClick() && (activeButton == 0)) || (btl.singleClick() && (activeButton == 1)))
    {
      game.setStrike();
    }
    if (millis() > act)
    {
      game.setStrike();
    }
    if (game.isState(ModuleState::STRIKED))
    {
      showEffekt(false);
      for (byte i = 0; i < 60; i++)
      {
        game.poll();
        delay(50);
      }
      initGame();
    }
    if (game.isState(ModuleState::DISARMED))
    {
      showEffekt(true);
      for (byte i = 0; i < 60; i++)
      {
        game.poll();
        delay(50);
      }
      initGame();
    }
  }
}

void showNeedy()
{
  if (changed)
  {
    u8x8.clearDisplay();
    byte t = random(2);
    byte a = random(2);
    if (t == 0)
      u8x8.drawString(0, 0, "    VENT GAS");
    else
      u8x8.drawString(0, 0, "    DETONATE");
    if (a == 0)
      u8x8.drawString(0, 2, "YES          NO");
    else
      u8x8.drawString(0, 2, "NO          YES");
    switch (a + t)
    {
    case 0:
    case 2:
      activeButton = 0; // button left
      break;
    case 1:
      activeButton = 1; // button right
    }
    changed = false;
  }
}

void poll()
{
  game.poll();
  if (game.isNewGameSettings())
  {
    initGame();
    game.arm();
  }
  btr.poll();
  btl.poll();
}

void showEffekt(bool solved)
{
  display.clear();
  u8x8.clearDisplay();
  if (solved)
  {
    u8x8.drawString(2, 1, "   SOLVED");
  }
  else
  {
    u8x8.drawString(2, 1, "   STRIKE");
  }
}
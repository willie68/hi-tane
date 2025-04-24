#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <shift7sDigit.h>
#include <avdweb_Switch.h>
#include <U8g2lib.h>

// operating in Wokwi
#define WOKWI
#define debug

#include <debug.h>
#include <game.h>
#include "indicators.h"
#include "communication.h"

// RGB LED
#define LED_PIN 4
// Game framework
Game game(ModuleTag::NEEDY_GAS, LED_PIN);

Switch btr = Switch(6); // Button right
Switch btl = Switch(5); // Button left

const byte latchPin = 8; // latch pin of the 74HC595
const byte clockPin = 9; // clock pin of the 74HC595
const byte dataPin = 7;  // data pin of the 74HC595

Shift7Segment display = Shift7Segment(2, dataPin, clockPin, latchPin);

U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* clock=A5*/ 19, /* data=A4*/ 18);

// Language Support :-)
// #define EN
#define DEU

#ifdef EN
// English
const char LB_NOTHING[] = "Nothing to do";
const char LB_VENTGAS[] = "    VENT GAS";
const char LB_DETONATE[] = "    DETONATE";
const char LB_YESNO[] = "YES          NO";
const char LB_NOYES[] = "NO          YES";
const char LB_SOLVED[] = "   SOLVED";
const char LB_STRIKE[] = "   STRIKE";
#endif
#ifdef DEU
// English
const char LB_NOTHING[] = "Nichts zu tun  ";
const char LB_VENTGAS[] = " Gas ablassen? ";
const char LB_DETONATE[] = "  Detonieren   ";
const char LB_YESNO[] = "Ja         Nein";
const char LB_NOYES[] = "Nein         Ja";
const char LB_SOLVED[] = "   RICHTIG";
const char LB_STRIKE[] = "   FALSCH";
#endif

// --- forward functions
void initDisplay();
void initGame();
void poll();
void showEffekt(bool solved);
void showNeedy();
void processWait();
void processUser();

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
}

void initDisplay()
{
  u8x8.begin();
  // u8x8.setFont(u8x8_font_courB18_2x3_f);
  u8x8.setFont(u8x8_font_8x13_1x2_f);
  u8x8.clearDisplay();
#ifndef WOKWI
  u8x8.setFlipMode(1);
#endif
  u8x8.setPowerSave(1);
  delay(1000);
  u8x8.setPowerSave(0);
}

unsigned long nextTime;
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
  nextTime = millis() + (1000 * waitSec);
  state = NS_WAIT;

  u8x8.clearDisplay();
  u8x8.drawString(2, 1, LB_NOTHING);
  changed = true;
  game.setSolved();
  game.setPixelColor(PX_BLUE);
}

void loop()
{
  poll();
  if (state == NS_WAIT)
    processWait();
  if (state == NS_USER)
    processUser();
}

void processWait()
{
  word timeValue = (nextTime - millis()) / 1000;
  #ifdef debug
  if (stimevalue != timeValue)
  {
    stimevalue = timeValue;
    dbgOut(F("wait tv: "));
    dbgOutLn(timeValue);
  }
  #endif
  // show the minutes to the next question
  if (btr.singleClick() || btl.singleClick())
  {
    byte min = timeValue / 60;
    Serial.print(min);
    Serial.println(F("min to wait"));
    display.showNumber(min);
    for (byte i = 0; i < 60; i++)
    {
      game.poll();
      delay(50);
    }
    display.clear();
  }
  if (millis() >= nextTime)
  {
    changed = true;
    state = NS_USER;
    nextTime = millis() + (1000L * userSec);
    game.sendBeep();
    game.arm();
  }
}

void processUser()
{
  byte timeValue = (nextTime - millis()) / 1000;
  if (stimevalue != timeValue)
  {
    stimevalue = timeValue;
    dbgOut(F("user tv: "));
    dbgOutLn(timeValue);
    display.showNumber(timeValue);
    if (timeValue <= 10)
    {
      game.sendBeep();
    }
  }
  showNeedy();
  // right button clicked
  if ((btr.singleClick() && (activeButton == 1)) || (btl.singleClick() && (activeButton == 0)))
  {
    game.setSolved();
  }
  // Wrong button clicked
  if ((btr.singleClick() && (activeButton == 0)) || (btl.singleClick() && (activeButton == 1)))
  {
    game.setStrike();
  }
  // Time over
  if (millis() > nextTime)
  {
    game.setStrike();
  }
  // strike, new game
  if (game.isState(ModuleState::STRIKED))
  {
    showEffekt(false);
    for (byte i = 0; i < 60; i++)
    {
      game.poll();
      delay(50);
    }
    initGame();
    return;
  }
  // solved, new game
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

void showNeedy()
{
  if (changed)
  {
    u8x8.clearDisplay();
    byte t = random(2);
    byte a = random(2);
    if (t == 0)
      u8x8.drawString(0, 0, LB_VENTGAS);
    else
      u8x8.drawString(0, 0, LB_DETONATE);
    if (a == 0)
      u8x8.drawString(0, 2, LB_YESNO);
    else
      u8x8.drawString(0, 2, LB_NOYES);
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
    u8x8.drawString(2, 1, LB_SOLVED);
  }
  else
  {
    u8x8.drawString(2, 1, LB_STRIKE);
  }
}
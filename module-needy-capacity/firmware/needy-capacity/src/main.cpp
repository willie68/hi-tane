#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <shift7sDigit.h>
#include <avdweb_Switch.h>
#include <SPI.h>
#include <Wire.h>
#define SSD1306_NO_SPLASH
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define debug // enter debug mode
#include <debug.h>
#include <game.h>
#include "indicators.h"
#include "communication.h"

// RGB LED
const byte LED_PIN = 4;
// Game framework
Game game(ModuleTag::NEEDY_CAPACITY, LED_PIN);

const byte BUTTON_PIN = 6; // Button

const byte latchPin = 8; // latch pin of the 74HC595
const byte clockPin = 9; // clock pin of the 74HC595
const byte dataPin = 7;  // data pin of the 74HC595

Shift7Segment sseg = Shift7Segment(2, dataPin, clockPin, latchPin);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- forward functions
void initDisplay();
void initGame();
void poll();
void showEffekt(bool solved);
void showNeedy();
void processWait();
void processActive();
void strike();
void fillPoly(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, int16_t c);
void showLevel(int fillLevel);
void showDigit(byte dg, byte v, byte y);
void showNumber(byte timeValue);
void showSegment(byte digit, byte seg, byte y);

enum NeedyState
{
  NS_INIT,
  NS_WAIT,
  NS_ACTIVE,
  NS_DONE
};

NeedyState state = NS_INIT;

void setup()
{
  Serial.begin(115200);
  Serial.println("init");
  randomSeed(analogRead(0));
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  initDisplay();

  game.init();

  initGame();
}

void initDisplay()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.setRotation(3);
  display.clearDisplay();
}

const long MAX_FILL = 10000;
const unsigned long cycleTime = 100; // cycletime in ms
const unsigned long DISCHARGE_MULTI = 5L;

unsigned long activeTime;
unsigned long stimevalue;
bool changed = true;
byte activeButton;
word waitSec = 10;
byte userSec = 90;
byte filllevel = 0;
long delta = 0;
long dischargeDelta = 0;

void initGame()
{
  state = NS_INIT;
  // initial wait time, after this the module starts working
  waitSec = random(60, 600);
  waitSec = 10;
  // userSec = 30;
  dbgOut(F("wait: "));
  dbgOutLn(waitSec);

  delta = (MAX_FILL * cycleTime / (userSec * 1000UL));
  dischargeDelta = DISCHARGE_MULTI * delta;
  dbgOut(F("charge with: "));
  dbgOut(delta);
  dbgOut(F(", discharge with: "));
  dbgOutLn(dischargeDelta);

  activeTime = millis() + (1000UL * waitSec);
  filllevel = 0;
  state = NS_WAIT;

  display.clearDisplay();
  changed = true;
  game.setSolved();
  game.setPixelColor(PX_BLUE);
}

void loop()
{
  poll();
  if (state == NS_WAIT)
  processWait();
  if (state == NS_ACTIVE)
    processActive();
}

// Wait until the module is active
void processWait()
{
#ifdef debug
  word timeValue = (activeTime - millis()) / 1000;
  if (stimevalue != timeValue)
  {
    stimevalue = timeValue;
    dbgOut(F("wait tv: "));
    dbgOutLn(timeValue);
    showNumber(timeValue);
  }
#endif
  if (millis() >= activeTime)
  {
    changed = true;
    state = NS_ACTIVE;
    activeTime = millis() + (1000L * userSec);
    game.sendBeep();
    game.arm();
  }
}

unsigned long lastCall = 0;
byte fillSec = 0;
long fillLevel = 0; // as 10000 means 100%
byte sFillTime = 0;
unsigned long sTime = 0;
// now the module is active and needs always some attention
void processActive()
{
  unsigned long actTime = millis();
  if (sTime < actTime)
  {
    sTime = actTime + cycleTime;

    byte fillTime = userSec - byte(fillLevel * long(userSec) / MAX_FILL);
    if (sFillTime != fillTime)
    {
      sFillTime = fillTime;
      showNumber(fillTime);
      sseg.showNumber(fillTime);
      dbgOut(F("fillTime: "));
      dbgOut(fillTime);
    }
    if (digitalRead(BUTTON_PIN))
    {
      game.setPixelColor(PX_RED);
      fillLevel = fillLevel + delta;
      dbgOut(F(", charge, "));
    }
    else
    {
      game.setPixelColor(PX_GREEN);
      fillLevel = fillLevel - dischargeDelta;
      dbgOut(F(", discharge, "));
    }
    fillLevel = min(fillLevel, MAX_FILL);
    fillLevel = max(0, fillLevel);
    dbgOutLn(fillLevel);
    showLevel(fillLevel / 100);
  }
  // Time over
  if (fillLevel >= MAX_FILL)
  {
    strike();
    state = NS_DONE;
  }
}

void strike()
{
  game.setStrike();
  display.clearDisplay();
  showDigit(0, 10, 0);
  showDigit(1, 10, 0);

  display.fillRoundRect(0, 27, 31, 100, 6, SSD1306_WHITE);

  display.fillTriangle(0, 82, 12, 76, 0, 76, SSD1306_BLACK);
  display.fillTriangle(0, 76, 12, 72, 12, 76, SSD1306_BLACK);

  display.fillTriangle(12, 76, 20, 80, 12, 72, SSD1306_BLACK);
  display.fillTriangle(12, 72, 20, 76, 20, 80, SSD1306_BLACK);

  display.fillTriangle(20, 80, 28, 74, 20, 76, SSD1306_BLACK);

  display.display();
  sseg.showSegments(0, 0x40);
  sseg.showSegments(1, 0x40);
}

void fillPoly(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, int16_t c)
{
  display.fillTriangle(x0, y0, x1, y1, x2, y2, c);
  display.fillTriangle(x0, y0, x3, y3, x2, y2, c);
}

void poll()
{
  game.poll();
  if (game.isNewGameSettings())
  {
    initGame();
  }
}

#define SEG_A 0
#define SEG_B 1
#define SEG_C 2
#define SEG_D 3
#define SEG_E 4
#define SEG_F 5
#define SEG_G 6
#define SEG_P 7

void showNumber(byte timeValue)
{
  display.fillRect(0, 0, 31, 27, SSD1306_BLACK);
  byte v = timeValue;
  showDigit(1, v % 10, 0);
  v = v / 10;
  showDigit(0, v % 10, 0);

  display.display();
}

void showDigit(byte dg, byte v, byte y)
{
  switch (v)
  {
  case 0:
    showSegment(dg, SEG_A, y);
    showSegment(dg, SEG_B, y);
    showSegment(dg, SEG_C, y);
    showSegment(dg, SEG_D, y);
    showSegment(dg, SEG_E, y);
    showSegment(dg, SEG_F, y);
    break;
  case 1:
    showSegment(dg, SEG_B, y);
    showSegment(dg, SEG_C, y);
    break;
  case 2:
    showSegment(dg, SEG_A, y);
    showSegment(dg, SEG_B, y);
    showSegment(dg, SEG_D, y);
    showSegment(dg, SEG_E, y);
    showSegment(dg, SEG_G, y);
    break;
  case 3:
    showSegment(dg, SEG_A, y);
    showSegment(dg, SEG_B, y);
    showSegment(dg, SEG_C, y);
    showSegment(dg, SEG_D, y);
    showSegment(dg, SEG_G, y);
    break;
  case 4:
    showSegment(dg, SEG_B, y);
    showSegment(dg, SEG_C, y);
    showSegment(dg, SEG_F, y);
    showSegment(dg, SEG_G, y);
    break;
  case 5:
    showSegment(dg, SEG_A, y);
    showSegment(dg, SEG_C, y);
    showSegment(dg, SEG_D, y);
    showSegment(dg, SEG_F, y);
    showSegment(dg, SEG_G, y);
    break;
  case 6:
    showSegment(dg, SEG_A, y);
    showSegment(dg, SEG_C, y);
    showSegment(dg, SEG_D, y);
    showSegment(dg, SEG_E, y);
    showSegment(dg, SEG_F, y);
    showSegment(dg, SEG_G, y);
    break;
  case 7:
    showSegment(dg, SEG_A, y);
    showSegment(dg, SEG_B, y);
    showSegment(dg, SEG_C, y);
    break;
  case 8:
    showSegment(dg, SEG_A, y);
    showSegment(dg, SEG_B, y);
    showSegment(dg, SEG_C, y);
    showSegment(dg, SEG_D, y);
    showSegment(dg, SEG_E, y);
    showSegment(dg, SEG_F, y);
    showSegment(dg, SEG_G, y);
    break;
  case 9:
    showSegment(dg, SEG_A, y);
    showSegment(dg, SEG_B, y);
    showSegment(dg, SEG_C, y);
    showSegment(dg, SEG_D, y);
    showSegment(dg, SEG_F, y);
    showSegment(dg, SEG_G, y);
    break;
  default:
    showSegment(dg, SEG_G, y);
    break;
  }
}

const byte SG_SIZE = 12;
void showSegment(byte digit, byte seg, byte y)
{
  byte x = (digit * SG_SIZE) + (digit * SG_SIZE / 2);

  switch (seg)
  {
  case SEG_A: // A
    display.drawFastHLine(x + 1, y, SG_SIZE - 2, SSD1306_WHITE);
    break;
  case SEG_B: // B
    display.drawFastVLine(x + SG_SIZE, y + 1, SG_SIZE - 2, SSD1306_WHITE);
    break;
  case SEG_C: // C
    display.drawFastVLine(x + SG_SIZE, y + 1 + SG_SIZE, SG_SIZE - 2, SSD1306_WHITE);
    break;
  case SEG_D: // D
    display.drawFastHLine(x + 1, y + (2 * SG_SIZE), SG_SIZE - 2, SSD1306_WHITE);
    break;
  case SEG_E: // E
    display.drawFastVLine(x, y + 1 + SG_SIZE, SG_SIZE - 2, SSD1306_WHITE);
    break;
  case SEG_F: // F
    display.drawFastVLine(x, y + 1, SG_SIZE - 2, SSD1306_WHITE);
    break;
  case SEG_G: // G
    display.drawFastHLine(x + 1, y + SG_SIZE, SG_SIZE - 2, SSD1306_WHITE);
    break;
  default:
    break;
  }
}

void showLevel(int fillLevel)
{
  display.fillRect(0, 27, 31, 100, SSD1306_BLACK);
  display.drawRoundRect(0, 27, 31, 100, 6, SSD1306_WHITE);
  display.fillRoundRect(0, 127 - fillLevel, 31, int16_t(fillLevel), 6, SSD1306_WHITE);
  display.display();
}
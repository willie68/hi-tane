#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <shift7sDigit.h>
#include <avdweb_Switch.h>
#include <SPI.h>
#include <Wire.h>
#define SSD1306_NO_SPLASH
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define debug
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
void showLevel(int fillLevel);
void showDigit(byte dg, byte v, byte y);
void showNumber(byte timeValue);
void showSegment(byte digit, byte seg, byte y);

enum NeedyState
{
  NS_INIT,
  NS_WAIT,
  NS_ACTIVE
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

unsigned long activeTime;
unsigned long stimevalue;
bool changed = true;
byte activeButton;
unsigned long waitSec = 10;
unsigned long userSec = 90;
byte filllevel = 0;

void initGame()
{
  state = NS_INIT;
  // initial wait time, after this the module starts working
  waitSec = random(180, 600);
#ifdef debug
  waitSec = 5;
//  userSec = 30;
#endif
  dbgOutLn("wait: ");
  dbgOutLn(waitSec);
  activeTime = millis() + (1000 * waitSec);
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
  word timeValue = (activeTime - millis()) / 1000;
#ifdef debug
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
const long MAX_FILL = 10000;
byte sFillTime = 0;
unsigned long sTime = 0;
const unsigned long cycleTime = 100;
const unsigned long DISCHARGE_MULTI = 5L;
// now the module is active and needs always some attention
void processActive()
{
  unsigned long actTime = millis();
  // right button clicked
  if (lastCall < actTime)
  {
    lastCall = actTime + 100;;
    byte fillTime = userSec - byte(fillLevel * long(userSec) / MAX_FILL);
    if (sFillTime != fillTime)
    {
      sFillTime = fillTime;
      showNumber(fillTime);
    }
  }
  if (sTime < actTime)
  {
    sTime = actTime + cycleTime;
    long delta = (60UL * cycleTime) / (long(userSec) * 10UL);
    if (digitalRead(BUTTON_PIN))
    {
      fillLevel = fillLevel + delta;
    }
    else
    {
      fillLevel = fillLevel - (DISCHARGE_MULTI * delta);
    }
    fillLevel = min(fillLevel, MAX_FILL);
    fillLevel = max(0, fillLevel);
    showLevel(fillLevel / 100);
  }
  // Time over
  if (fillLevel >= MAX_FILL)
  {
    game.setStrike();
    display.clearDisplay();
    showDigit(0, 10, 0);
    showDigit(1, 10, 0);
    display.fillRoundRect(0, 27, 31, 100, 6, SSD1306_WHITE);
    display.display();
    while (true)
    {
      poll();
    }
  }
}

void poll()
{
  game.poll();
  if (game.isNewGameSettings())
  {
    initGame();
  }
}

#define SA 0
#define SB 1
#define SC 2
#define SD 3
#define SE 4
#define SF 5
#define SG 6
#define SP 7

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
    showSegment(dg, SA, y);
    showSegment(dg, SB, y);
    showSegment(dg, SC, y);
    showSegment(dg, SD, y);
    showSegment(dg, SE, y);
    showSegment(dg, SF, y);
    break;
  case 1:
    showSegment(dg, SB, y);
    showSegment(dg, SC, y);
    break;
  case 2:
    showSegment(dg, SA, y);
    showSegment(dg, SB, y);
    showSegment(dg, SD, y);
    showSegment(dg, SE, y);
    showSegment(dg, SG, y);
    break;
  case 3:
    showSegment(dg, SA, y);
    showSegment(dg, SB, y);
    showSegment(dg, SC, y);
    showSegment(dg, SD, y);
    showSegment(dg, SG, y);
    break;
  case 4:
    showSegment(dg, SB, y);
    showSegment(dg, SC, y);
    showSegment(dg, SF, y);
    showSegment(dg, SG, y);
    break;
  case 5:
    showSegment(dg, SA, y);
    showSegment(dg, SC, y);
    showSegment(dg, SD, y);
    showSegment(dg, SF, y);
    showSegment(dg, SG, y);
    break;
  case 6:
    showSegment(dg, SA, y);
    showSegment(dg, SC, y);
    showSegment(dg, SD, y);
    showSegment(dg, SE, y);
    showSegment(dg, SF, y);
    showSegment(dg, SG, y);
    break;
  case 7:
    showSegment(dg, SA, y);
    showSegment(dg, SB, y);
    showSegment(dg, SC, y);
    break;
  case 8:
    showSegment(dg, SA, y);
    showSegment(dg, SB, y);
    showSegment(dg, SC, y);
    showSegment(dg, SD, y);
    showSegment(dg, SE, y);
    showSegment(dg, SF, y);
    showSegment(dg, SG, y);
    break;
  case 9:
    showSegment(dg, SA, y);
    showSegment(dg, SB, y);
    showSegment(dg, SC, y);
    showSegment(dg, SD, y);
    showSegment(dg, SF, y);
    showSegment(dg, SG, y);
    break;
  default:
    showSegment(dg, SG, y);
    break;
  }
}

const byte SG_SIZE = 12;
void showSegment(byte digit, byte seg, byte y)
{
  byte x = (digit * SG_SIZE) + (digit * SG_SIZE / 2);

  switch (seg)
  {
  case SA: // A
    display.drawFastHLine(x + 1, y, SG_SIZE - 2, SSD1306_WHITE);
    break;
  case SB: // B
    display.drawFastVLine(x + SG_SIZE, y + 1, SG_SIZE - 2, SSD1306_WHITE);
    break;
  case SC: // C
    display.drawFastVLine(x + SG_SIZE, y + 1 + SG_SIZE, SG_SIZE - 2, SSD1306_WHITE);
    break;
  case SD: // D
    display.drawFastHLine(x + 1, y + (2 * SG_SIZE), SG_SIZE - 2, SSD1306_WHITE);
    break;
  case SE: // E
    display.drawFastVLine(x, y + 1 + SG_SIZE, SG_SIZE - 2, SSD1306_WHITE);
    break;
  case SF: // F
    display.drawFastVLine(x, y + 1, SG_SIZE - 2, SSD1306_WHITE);
    break;
  case SG: // G
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
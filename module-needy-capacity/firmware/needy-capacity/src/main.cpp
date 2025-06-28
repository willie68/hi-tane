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

Switch btr = Switch(6); // Button right
Switch btl = Switch(5); // Button left

const byte latchPin = 8; // latch pin of the 74HC595
const byte clockPin = 9; // clock pin of the 74HC595
const byte dataPin = 7;  // data pin of the 74HC595

Shift7Segment sseg = Shift7Segment(2, dataPin, clockPin, latchPin);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
void showNumber(byte timeValue);
void showDigit(byte dg, byte v, byte y);
void showSegment(byte digit, byte seg, byte y);

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
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.setRotation(3);
  display.clearDisplay();
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
    showNumber(timeValue);
  }
#endif
  // show the minutes to the next question
  if (btr.singleClick() || btl.singleClick())
  {
    byte min = timeValue / 60;
    Serial.print(min);
    Serial.println(F("min to wait"));
    sseg.showNumber(min);
    for (byte i = 0; i < 60; i++)
    {
      game.poll();
      delay(50);
    }
    sseg.clear();
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
    sseg.showNumber(timeValue);
    showNumber(timeValue);
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
    display.clearDisplay();
    byte t = random(2);
    byte a = random(2);
    /*
    if (t == 0)
    u8x8.drawString(0, 0, LB_VENTGAS);
    else
    u8x8.drawString(0, 0, LB_DETONATE);
    if (a == 0)
    u8x8.drawString(0, 2, LB_YESNO);
    else
    u8x8.drawString(0, 2, LB_NOYES);
    */
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
  sseg.clear();
  display.clearDisplay();
  /*
  if (solved)
  {
    u8x8.drawString(2, 1, LB_SOLVED);
  }
  else
  {
    u8x8.drawString(2, 1, LB_STRIKE);
  }
  */
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
  display.clearDisplay();
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
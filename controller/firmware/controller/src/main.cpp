#include <Arduino.h>
#include <TM1637Display.h>
#include <Adafruit_NeoPixel.h>
#include <U8g2lib.h>
#include <ClickEncoder.h>
#include <timerOne.h>
#include <avr/wdt.h>

// #define debug
#include <debug.h>
#include <globals.h>
#include "indicators.h"
#include "communication.h"
#include "serialnumber.h"
#include <game.h>
#include <pins_arduino.h>
#include <display.h>

// 7 seg  Display
#define CLK 3
#define DIO 2

// 7-Segment LED Display
const uint8_t TTD[] = {SEG_F | SEG_G | SEG_E | SEG_D};
const uint8_t MND[] = {SEG_G};

TM1637Display seg7 = TM1637Display(CLK, DIO);

const int MAX_TIME = 90 * 60;

// Encoder
const uint8_t PIN_ENCA = 6;
const uint8_t PIN_ENCB = 7;
const uint8_t PIN_BTN = 5;
const uint8_t ENC_STEPSPERNOTCH = 4;
const bool BTN_ACTIVESTATE = LOW;
const uint16_t TIMER_NOTIFY_US = 1000;
constexpr uint8_t PRINT_BASE = 10;
static TimerOne timer;
ClickEncoder clickEnc{PIN_ENCA, PIN_ENCB, PIN_BTN, ENC_STEPSPERNOTCH, BTN_ACTIVESTATE};

// LCD definitions
HIDisplay display;

// RGB LED
#define LED_PIN 4
Adafruit_NeoPixel pixel(3, LED_PIN, NEO_RGB + NEO_KHZ800);
const uint32_t RED = pixel.Color(0xff, 0, 0);
const uint32_t BLUE = pixel.Color(0, 0, 0xff);
const uint32_t GREEN = pixel.Color(0, 0xff, 0);

// Tone
const uint8_t BEEP_PIN = 9;

// --- forward-declared function prototypes:
void showTime(int act);

void beep();
void dblBeep();
void hibeep();
void initHTCOM();
void initGame();
void resetStrikes();
void showStrikes();
void startGame();
void calculateActGameTime();
void showMenu();
void menuSetBrightness();
void menuSetDifficulty();
void menuSetGameTime();
void gt2Display();
void LED(bool on);
bool checkResolved();
bool checkFullyStriked();
void showResolved();
void showFullyStriked();
void reset();
void printModules();

// Communication
HTCOM htcom;
Indicators indicators;
SerialNumber serialnumber;
char buffer[30];

uint32_t color = BLUE;
int gameTime = 60 * 60;
long start;
Difficulty difficulty = Difficulty::SIMPLE;
Difficulty sdiff = Difficulty::SIMPLE;
bool started, paused;

void timerIsr()
{
  // This is the Encoder's worker routine. It will physically read the hardware
  // and all most of the logic happens here. Recommended interval for this method is 1ms.
  clickEnc.service();
}

void setup()
{
  initDebug();
  dbgOutLn(F("init"));
  pinMode(LED_BUILTIN, OUTPUT);
  LED(1);
  dbgOutLn(F("init 7seg"));
  seg7.clear();
  seg7.setSegments(TTD, 1, 0);
  seg7.setBrightness(DEFAULT_BRIGHTNESS > 1);

  dbgOutLn(F("init neopixel"));
  pixel.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixel.setBrightness(DEFAULT_BRIGHTNESS * 16);
  pixel.show();

  // Setup and configure "full-blown" ClickEncoder
  dbgOutLn(F("init encoder"));
  clickEnc.setAccelerationEnabled(false);
  clickEnc.setDoubleClickEnabled(true);
  clickEnc.setLongPressRepeatEnabled(false);

  dbgOutLn(F("init timer"));
  timer.initialize(TIMER_NOTIFY_US);
  timer.attachInterrupt(timerIsr);
  start = 0;
  randomSeed(analogRead(0));

  dbgOutLn(F("init display"));
  display.init(htcom, indicators, serialnumber);
  display.clear();

  dbgOutLn(F("init game"));
  initGame();
  LED(0);
}

void initGame()
{
  started = false;
  paused = false;
  display.clear();

  dbgOutLn(F("print header"));
  display.printHeader(true);
  dbgOutLn(F("print welcome"));
  display.printWelcome();

  dbgOutLn(F("generate serial number"));
  serialnumber.Generate();
#ifdef debug
  dbgOut(F("Serialnumber is: "));
  serialnumber.String(buffer);
  dbgOut(buffer);
  dbgOutLn();
#endif

  dbgOutLn(F("generate indicators"));
  indicators.Generate();

  dbgOutLn("htcom init");
  initHTCOM();
  resetStrikes();

  dbgOutLn("htcom init ready");
  delay(1000);
}

void initHTCOM()
{
  htcom = HTCOM();
  htcom.attach(ID_CONTROLLER);
  htcom.sendCtrlInitialisation();
  htcom.setCtrlSerialNumber(serialnumber.Get());
  htcom.setCtrlDifficulty(difficulty);
  htcom.setCtrlIndicators(indicators.Compress());
}

// menu line to display, with save var
int8_t line = 0;
byte sline = 255;

// save error number
byte serr = 0;

bool cmdStartGame = true;
bool resolved = false;
bool fullyStriked = false;
byte brightness = DEFAULT_BRIGHTNESS;
byte sbr = 0;
int sgt = 0;
int act;       // actual game time
byte mcnt = 0; // module count

void loop()
{
  htcom.poll();
  if (started && !(paused))
  {
    calculateActGameTime();
    showTime(act);
    if (htcom.isBeep())
    {
      dbgOutLn(F("beep from external"));
      beep();
    }
  }
  if (htcom.hasCtrlError())
  {
    byte err = htcom.getCtrlError();
    if (serr != err)
    {
      serr = err;
      display.printError(err);
    }
  }
  else
  {
    if (serr > 0)
    {
      serr = 0;
      display.clearError();
    }
  }
  if (!started)
    showMenu();

  if (started)
  {
    showStrikes();
    resolved = checkResolved();
    fullyStriked = checkFullyStriked();
    if (resolved || fullyStriked)
    {
      started = false;
    }
  }
  if (resolved)
    showResolved();

  if (fullyStriked)
    showFullyStriked();
}

void calculateActGameTime()
{
  act = gameTime - int(((millis() - start) / 1000L));
  if (act < 0)
  {
    act = 0;
  }
}

void showMenu()
{
  if (line != sline)
  {
    sline = line;
    display.clearrow(1);
    display.setCursor(0, 1);
    switch (sline)
    {
    case 0:
      display.print(F("start Game? yes"));
      display.setCursor(12, 1);
      display.cursor();
      display.noBlink();
      break;
    case 1:
      display.print(F("Difficulty:"));
      display.setCursor(12, 1);
      strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[difficulty])));
      display.print(buffer);
      display.setCursor(12, 1);
      display.cursor();
      display.noBlink();
      break;
    case 2:
      display.print(F("Brigthness:"));
      display.setCursor(12, 1);
      display.print(brightness);
      display.print(" ");
      display.setCursor(12, 1);
      display.cursor();
      display.noBlink();
      break;
    case 3:
      display.print(F("Gametime:  "));
      gt2Display();
      display.cursor();
      display.noBlink();
      break;
    }
  }
  switch (sline)
  {
  case 0:
    if (clickEnc.getButton() == Button::Clicked)
    {
      display.clearrow(1);
      display.noCursor();
      display.noBlink();
      startGame();
    }
    break;
  case 1:
    if (difficulty != sdiff)
    {
      sdiff = difficulty;
      display.setCursor(12, 1);
      strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[difficulty])));
      display.print(buffer);
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      menuSetDifficulty();
      display.setCursor(12, 1);
      display.noBlink();
    }
    break;
  case 2:
    if (brightness != sbr)
    {
      sbr = brightness;
      display.setCursor(12, 1);
      display.print(brightness);
      display.print(" ");
      display.setCursor(12, 1);
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      menuSetBrightness();
      display.setCursor(12, 1);
      display.noBlink();
    }
    break;
  case 3:
    if (gameTime != sgt)
    {
      sgt = gameTime;
      gt2Display();
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      menuSetGameTime();
      display.setCursor(10, 1);
      display.noBlink();
    }
    break;
  }
  int16_t value = clickEnc.getIncrement();
  if (value != 0)
  {
    value > 0 ? line++ : line--;
    if (line > 3)
      line = 3;
    if (line < 0)
      line = 0;
  }
}

void menuSetDifficulty()
{
  display.setCursor(12, 1);
  display.blink();
  sbr = 0;
  bool ok = false;
  while (!ok)
  {
    htcom.poll();
    int16_t value = clickEnc.getIncrement();
    if (value != 0)
    {
      value > 0 ? nextDiff(difficulty) : prevDiff(difficulty);
    }
    if (difficulty != sdiff)
    {
      sdiff = difficulty;
      strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[difficulty])));
      display.setCursor(12, 1);
      display.print(buffer);
      display.setCursor(12, 1);
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      htcom.setCtrlDifficulty(difficulty);
      ok = true;
    }
  }
  display.noBlink();
}

void menuSetBrightness()
{
  display.setCursor(12, 1);
  display.blink();
  sbr = 0;
  bool ok = false;
  while (!ok)
  {
    htcom.poll();
    int16_t value = clickEnc.getIncrement();
    if (value != 0)
    {
      value > 0 ? brightness++ : brightness--;
      if (brightness > 15)
        brightness = 15;
      if (brightness < 1)
        brightness = 1;
    }
    if (brightness != sbr)
    {
      sbr = brightness;
      display.setCursor(12, 1);
      display.print(brightness);
      display.print(" ");
      display.setCursor(12, 1);
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      htcom.setCtrlBrightness(brightness);
      pixel.setBrightness(brightness * 16);
      ok = true;
    }
  }
  display.noBlink();
}

void menuSetGameTime()
{
  display.setCursor(10, 1);
  display.blink();
  sgt = 0;
  bool ok = false;
  while (!ok)
  {
    htcom.poll();
    int16_t value = clickEnc.getIncrement();
    if (value != 0)
    {
      value > 0 ? gameTime += 60 : gameTime -= 60;
      if (gameTime > MAX_TIME)
        gameTime = MAX_TIME;
      if (gameTime < 1)
        gameTime = 60;
    }
    if (gameTime != sgt)
    {
      sgt = gameTime;
      gt2Display();
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      ok = true;
    }
  }
  display.noBlink();
}

void gt2Display()
{
  display.setCursor(10, 1);
  display.print(gameTime / 60);
  display.print(" min ");
  display.setCursor(10, 1);
}

void startGame()
{
  dbgOutLn(F("start game"));
  resetStrikes();
  pixel.setBrightness(16 * htcom.getBrightness());
  seg7.setBrightness(htcom.getBrightness() >> 1);
  display.setBrightness(16 * htcom.getBrightness());
  
  htcom.initModules();
  htcom.sendCtrlInitialisation();
  byte count = 100;
  while (count > 0)
  {
    count--;
    htcom.poll();
    delay(10);
  }
  htcom.setCtrlDifficulty(difficulty);
  htcom.sendCtrlGameSettings();

  display.printHeader(false);

  dblBeep();
  started = true;
  resolved = false;
  start = millis();
  calculateActGameTime();
  mcnt = htcom.installedModuleCount();
  if (mcnt == 0)
  {
    dbgOutLn(F("no module installed, faking"));
    htcom.addTestModule();
  }
#ifdef debug
  dbgOut(F("found "));
  dbgOut2(mcnt, DEC);
  dbgOutLn(F(" modules"));

  for (byte idx = 0; idx < mcnt; idx++)
  {
    byte mod = htcom.getInstalledModuleID(idx);

    strcpy_P(buffer, (char *)pgm_read_word(&(MODULE_LABELS[mod - MOD_OFFSET])));
    dbgOutLn(buffer);
  }

#endif
  // printModules();

  dbgOutLn(F("print status"));
  display.printStatus();
}

int saveTime = 0;
void showTime(int act)
{
  if (act != saveTime)
  {
    saveTime = act;
    // printModules();

    htcom.sendCtrlHearbeat(act);
    display.showTime(act);

    bool neg = act < 0;
    int t = abs(act);
    byte sec = t % 60;
    byte min = (t - sec) / 60;
    if (neg)
    {
      seg7.setSegments(MND, 1, 0);
      seg7.showNumberDec(min, false, 1, 1);
    }
    else
    {
      seg7.showNumberDecEx(min, 0b01000000 & (sec % 2) << 6, false, 2, 0);
    }
    seg7.showNumberDec(sec, true, 2, 2);
    if ((act % 900) == 0)
    {
      dblBeep();
    }
    if ((act <= 10) && (act > 0))
    {
      beep();
    }
    if (act == 0)
    {
      hibeep();
    }
  }
}

void dblBeep()
{
  tone(BEEP_PIN, 440, 100);
  delay(200);
  tone(BEEP_PIN, 440, 100);
  delay(100);
  pinMode(BEEP_PIN, INPUT);
}

void beep()
{
  tone(BEEP_PIN, 440, 100);
  delay(100);
  pinMode(BEEP_PIN, INPUT);
}

void hibeep()
{
  tone(BEEP_PIN, 880, 300);
  delay(300);
  pinMode(BEEP_PIN, INPUT);
}

void resetStrikes()
{
  htcom.setCtlrStrikes(0);
  fullyStriked = false;
}

void showStrikes()
{
  display.showStrikes();
  byte strikes = htcom.getStrikes();
  for (uint8_t x = 0; x < 3; x++)
  {
    pixel.setPixelColor(x, GREEN);
    if (x < strikes)
      pixel.setPixelColor(x, RED);
  }
  pixel.show();
}

void LED(bool on)
{
  digitalWrite(LED_BUILTIN, on);
}

bool checkResolved()
{
  return htcom.isAllResolved();
}

bool checkFullyStriked()
{
  return (act == 0) || (htcom.getStrikes() == 3);
}

void showResolved()
{
  display.clear();
  display.printHeader(true);
  display.resolved();

  beep();
  while (true)
  {
    if (clickEnc.getButton() == Button::Clicked)
    {
      reset();
      break;
    }
  }
}

void showFullyStriked()
{
  display.clear();
  display.printHeader(true);
  display.fullyStriked();

  beep();
  while (true)
  {
    if (clickEnc.getButton() == Button::Clicked)
    {
      reset();
      break;
    }
  }
}

void reset()
{
  wdt_disable();
  wdt_enable(WDTO_15MS);
  while (1)
  {
  }
}
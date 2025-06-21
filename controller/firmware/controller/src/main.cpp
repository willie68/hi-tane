#include <Arduino.h>
#include <TM1637Display.h>
#include <Adafruit_NeoPixel.h>
#include <U8g2lib.h>
#include <ClickEncoder.h>
#include <timerOne.h>
#include <avr/wdt.h>

#define debug
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
void longbeep();
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
void menuPaused();
void pixelHide();

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
  randomSeed(analogRead(0));

  htcom = HTCOM();
  htcom.attach(ID_CONTROLLER);

  dbgOutLn(F("init display"));
  display.init(htcom, indicators, serialnumber);
  display.clear();

  dbgOutLn(F("init game"));
  initGame();
  LED(0);
}

// menu line to display, with save var
int8_t line = 0;
int8_t sline = 255;

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

void initGame()
{
  sline = 255;
  start = 0;
  started = false;
  paused = false;
  display.clear();
  seg7.clear();
  pixelHide();

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
  htcom.sendCtrlInitialisation();
  htcom.setCtrlSerialNumber(serialnumber.Get());
  htcom.setCtrlDifficulty(difficulty);
  htcom.setCtrlIndicators(indicators.Compress());
}

void loop()
{
  htcom.poll();
  if (started && !(paused))
  {
    if (!paused)
    {
      calculateActGameTime();
      showTime(act);
      if (htcom.isBeep())
      {
        dbgOutLn(F("beep from external"));
        beep();
      }
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      if (!paused)
      {
        menuPaused();
      }
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

int8_t mpv, smpv; // menu paused value

void menuPaused()
{
  longbeep();
  paused = true;
  htcom.setCtrlGamePaused(paused);
  display.hideStatus();
  display.showMenuKey(4);

  mpv = 0;
  smpv = -1;
  long st = millis();

  while (paused)
  {
    htcom.poll();
    if (mpv != smpv)
    {
      display.showValueCursor();
      display.setCursorToValue();
      strcpy_P(buffer, (char *)pgm_read_word(&(PAUSE_MENU_VALUES[mpv])));
      display.print(buffer);
      dbgOut(F("pause menu: "));
      dbgOutLn(buffer);
      smpv = mpv;
    }
    int16_t value = clickEnc.getIncrement();
    if (value != 0)
    {
      value > 0 ? mpv++ : mpv--;
      if (mpv > 1)
        mpv = 1;
      if (mpv < 0)
        mpv = 0;
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      paused = false;
    }
  }
  if (mpv == 1)
  {
    dblBeep();
    initGame();
    return;
  }
  beep();
  htcom.setCtrlGamePaused(paused);
  display.hideMenu();
  display.printStatus();
  long delta = millis() - st;
  start = start + delta;
}

void showMenu()
{
  if (line != sline)
  {
    sline = line;
    display.showMenuKey(sline);
    switch (sline)
    {
    case 0:
      display.showValueCursor();
      display.setCursorToValue();
      display.print(F(lb_yes));
      break;
    case 1:
      display.hideValueCursor();
      display.setCursorToValue();
      strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[difficulty])));
      display.print(buffer);
      break;
    case 2:
      display.hideValueCursor();
      display.setCursorToValue();
      display.print(brightness);
      break;
    case 3:
      display.hideValueCursor();
      gt2Display();
      break;
    }
  }
  // display.showValueCursor();

  switch (sline)
  {
  case 0:
    if (clickEnc.getButton() == Button::Clicked)
    {
      display.hideMenu();
      startGame();
    }
    break;
  case 1:
    if (difficulty != sdiff)
    {
      sdiff = difficulty;
      display.setCursorToValue();
      strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[difficulty])));
      display.print(buffer);
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      menuSetDifficulty();
      display.hideValueCursor();
      display.noBlink();
    }
    break;
  case 2:
    if (brightness != sbr)
    {
      sbr = brightness;
      display.setCursorToValue();
      display.print(brightness);
      display.print(" ");
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      menuSetBrightness();
      display.hideValueCursor();
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
      display.hideValueCursor();
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
  display.showValueCursor();
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
      display.setCursorToValue();
      display.print(buffer);
      display.showValueCursor();
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      htcom.setCtrlDifficulty(difficulty);
      ok = true;
    }
  }
  display.noBlink();
  display.printHeader(true);
}

void menuSetBrightness()
{
  display.showValueCursor();
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
      display.setCursorToValue();
      display.print(brightness);
      display.showValueCursor();
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      htcom.setCtrlBrightness(brightness);
      pixel.setBrightness(brightness * 16);
      display.setBrightness(brightness);
      ok = true;
    }
  }
  display.noBlink();
}

void menuSetGameTime()
{
  display.setCursorToValue();
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
      display.showValueCursor();
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      ok = true;
    }
  }
  display.hideValueCursor();
  display.noBlink();
}

void gt2Display()
{
  display.setCursorToValue();
  display.print(gameTime / 60);
  display.print(lb_short_minutes);
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

void longbeep()
{
  tone(BEEP_PIN, 440, 500);
  delay(500);
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

void pixelHide()
{
  pixel.clear();
  pixel.show();
};
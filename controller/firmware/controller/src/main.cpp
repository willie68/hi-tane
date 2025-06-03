#include <Arduino.h>
#include <TM1637Display.h>
#include <Adafruit_NeoPixel.h>
#include <ClickEncoder.h>
#include <timerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>

// #define debug
#include <debug.h>
#include "indicators.h"
#include "communication.h"
#include <game.h>
#include <serialnumber.h>
#include <pins_arduino.h>

#define Version "V0.1 " // always add an space at the end

// Display
#define CLK 3
#define DIO 2

// 7-Segment LED Display
const uint8_t TTD[] = {SEG_F | SEG_G | SEG_E | SEG_D};
const uint8_t MND[] = {SEG_G};

TM1637Display display = TM1637Display(CLK, DIO);

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
const uint8_t COLUMS = 20;
const uint8_t ROWS = 4;
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

// RGB LED
#define LED_PIN 4
Adafruit_NeoPixel pixel(3, LED_PIN, NEO_RGB + NEO_KHZ800);
const uint32_t RED = pixel.Color(0xff, 0, 0);
const uint32_t BLUE = pixel.Color(0, 0, 0xff);
const uint32_t GREEN = pixel.Color(0, 0xff, 0);

// Tone
const uint8_t BEEP_PIN = 9;
// Communication
HTCOM htcom;

// --- forward-declared function prototypes:
void showTime(int act);

void clearRow(uint8_t row);
void beep();
void dblBeep();
void hibeep();
void generateSerialNumber();
void generateIndicators();
void printStatusLine();
void printHeader(bool withVersion);
void printWelcome();
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

Indicators indicators;
SerialNumber snr;
uint32_t color = BLUE;
int gameTime = 60 * 60;
long start;
char buffer[30];
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
  Serial.begin(115200);
  Serial.println("init");
  pinMode(LED_BUILTIN, OUTPUT);
  LED(1);
  display.clear();
  display.setSegments(TTD, 1, 0);
  display.setBrightness(DEFAULT_BRIGHTNESS > 1);

  pixel.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixel.setBrightness(DEFAULT_BRIGHTNESS * 16);
  pixel.show();

  // Setup and configure "full-blown" ClickEncoder
  clickEnc.setAccelerationEnabled(false);
  clickEnc.setDoubleClickEnabled(true);
  clickEnc.setLongPressRepeatEnabled(false);

  timer.initialize(TIMER_NOTIFY_US);
  timer.attachInterrupt(timerIsr);
  start = 0;
  randomSeed(analogRead(0));

  while (lcd.begin(COLUMS, ROWS, LCD_5x8DOTS) != 1) // colums, rows, characters size
  {
    Serial.println(F("PCF8574 not connected or lcd pins is wrong."));
    delay(5000);
  }

  lcd.clear();

  initGame();
  LED(0);
}

void initGame()
{
  started = false;
  paused = false;
  lcd.clear();

  printHeader(true);
  printWelcome();

  generateSerialNumber();

  generateIndicators();

  printStatusLine();

  //  htcom = HTCOM(COM_PIN, 44);
  dbgOutLn("htcom init");
  htcom = HTCOM();
  htcom.attach(ID_CONTROLLER);
  htcom.sendCtrlInitialisation();
  htcom.setCtrlSerialNumber(snr.Get());
  htcom.setCtrlDifficulty(difficulty);
  htcom.setCtrlIndicators(indicators.Compress());

  resetStrikes();

  dbgOutLn("htcom init ready");
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
      clearRow(2);
      strcpy_P(buffer, (char *)pgm_read_word(&(ERROR_MESSAGES[err])));
      lcd.print(buffer);
    }
  }
  else
  {
    if (serr > 0)
    {
      serr = 0;
      clearRow(2);
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
  {
    showResolved();
  }
  if (fullyStriked)
  {
    showFullyStriked();
  }
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
    clearRow(1);
    lcd.setCursor(0, 1);
    switch (sline)
    {
    case 0:
      lcd.print(F("start Game? yes"));
      lcd.setCursor(12, 1);
      lcd.cursor();
      lcd.noBlink();
      break;
    case 1:
      lcd.print(F("Difficulty:"));
      lcd.setCursor(12, 1);
      strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[difficulty])));
      lcd.print(buffer);
      lcd.setCursor(12, 1);
      lcd.cursor();
      lcd.noBlink();
      break;
    case 2:
      lcd.print(F("Brigthness:"));
      lcd.setCursor(12, 1);
      lcd.print(brightness);
      lcd.print(" ");
      lcd.setCursor(12, 1);
      lcd.cursor();
      lcd.noBlink();
      break;
    case 3:
      lcd.print(F("Gametime:  "));
      gt2Display();
      lcd.cursor();
      lcd.noBlink();
      break;
    }
  }
  switch (sline)
  {
  case 0:
    if (clickEnc.getButton() == Button::Clicked)
    {
      clearRow(1);
      lcd.noCursor();
      lcd.noBlink();
      startGame();
    }
    break;
  case 1:
    if (difficulty != sdiff)
    {
      sdiff = difficulty;
      lcd.setCursor(12, 1);
      strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[difficulty])));
      lcd.print(buffer);
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      menuSetDifficulty();
      lcd.setCursor(12, 1);
      lcd.noBlink();
    }
    break;
  case 2:
    if (brightness != sbr)
    {
      sbr = brightness;
      lcd.setCursor(12, 1);
      lcd.print(brightness);
      lcd.print(" ");
      lcd.setCursor(12, 1);
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      menuSetBrightness();
      lcd.setCursor(12, 1);
      lcd.noBlink();
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
      lcd.setCursor(10, 1);
      lcd.noBlink();
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
  lcd.setCursor(12, 1);
  lcd.blink();
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
      lcd.setCursor(12, 1);
      lcd.print(buffer);
      lcd.setCursor(12, 1);
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      htcom.setCtrlDifficulty(difficulty);
      ok = true;
    }
  }
  lcd.noBlink();
}

void menuSetBrightness()
{
  lcd.setCursor(12, 1);
  lcd.blink();
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
      lcd.setCursor(12, 1);
      lcd.print(brightness);
      lcd.print(" ");
      lcd.setCursor(12, 1);
    }
    if (clickEnc.getButton() == Button::Clicked)
    {
      htcom.setCtrlBrightness(brightness);
      pixel.setBrightness(brightness * 16);
      ok = true;
    }
  }
  lcd.noBlink();
}

void menuSetGameTime()
{
  lcd.setCursor(10, 1);
  lcd.blink();
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
  lcd.noBlink();
}

void gt2Display()
{
  lcd.setCursor(10, 1);
  lcd.print(gameTime / 60);
  lcd.print(" min ");
  lcd.setCursor(10, 1);
}

void startGame()
{
  dbgOutLn(F("start game"));
  resetStrikes();
  pixel.setBrightness(16 * htcom.getBrightness());
  display.setBrightness(htcom.getBrightness() >> 1);
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

  clearRow(0);
  printHeader(false);

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
  printModules();
}

void printModules()
{
  clearRow(1);
  clearRow(2);
  dbgOut(F("print modules "));
  dbgOutLn2(mcnt, DEC);
  lcd.setCursor(0, 1);
  for (byte idx = 0; idx < mcnt; idx++)
  {
    byte mod = htcom.getInstalledModuleID(idx);
    if (mod != ID_NONE)
    {
      strcpy_P(buffer, (char *)pgm_read_word(&(MODULE_LABELS[mod - MOD_OFFSET])));
      lcd.print(buffer);
      if (htcom.isModuleState(mod, ModuleState::DISARMED))
      {
        lcd.print(F(" "));
      }
      else
      {
        lcd.print(F("*"));
      }
    }
    if (idx >= 5)
    {
      lcd.setCursor(0, 2);
    }
  }
}

void generateSerialNumber()
{
  snr.Generate();
  Serial.print("Serialnumber is: ");
  snr.String(buffer);
  Serial.println(buffer);
}

void generateIndicators()
{
  byte indCount = random(1, 4);
#ifdef debug
  Serial.print("ind: ");
  Serial.print(indCount);
  Serial.println();
#endif

  for (byte x = 0; x < indCount; x++)
  {
    indicators.Add(random(INDICATOR_COUNT) + 1, random(2) == 0);
  }
#ifdef debug
  for (byte x = 0; x < indicators.Count(); x++)
  {
    byte idx = indicators.Get(x);
    strcpy_P(buffer, (char *)pgm_read_word(&(INDICATORNAMES[idx])));
    Serial.print(idx);
    Serial.print(" ");
    if (indicators.IsActive(x))
    {
      Serial.print("#");
    }
    Serial.println(buffer);
  }
  Serial.println(indicators.Compress(), BIN);
#endif
}

void printStatusLine()
{
  clearRow(3);
  snr.String(buffer);
  lcd.print(buffer);

  for (byte x = 0; x < indicators.Count(); x++)
  {
    byte idx = indicators.Get(x);
    lcd.print(" ");
    if (indicators.IsActive(x))
    {
      lcd.print(char(0x23));
    }
    else
    {
    }
    strcpy_P(buffer, (char *)pgm_read_word(&(INDICATORNAMES[idx])));
    lcd.print(buffer);
  }
}

int saveTime = 0;
void showTime(int act)
{
  if (act != saveTime)
  {
    saveTime = act;
    printModules();

    htcom.sendCtrlHearbeat(act);

    lcd.setCursor(15, 0);
    bool neg = act < 0;
    int t = abs(act);
    byte sec = t % 60;
    byte min = (t - sec) / 60;
    if (neg)
    {
      lcd.print("-");
      display.setSegments(MND, 1, 0);
      if (min <= 0)
        lcd.print("0");
      lcd.print(min);
      display.showNumberDec(min, false, 1, 1);
    }
    else
    {
      if (min <= 9)
        lcd.print("0");
      lcd.print(min);
      display.showNumberDecEx(min, 0b01000000 & (sec % 2) << 6, false, 2, 0);
    }
    lcd.print(":");
    if (sec <= 9)
      lcd.print("0");
    lcd.print(sec);
    display.showNumberDec(sec, true, 2, 2);
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

void clearRow(uint8_t row)
{
  lcd.setCursor(0, row);
  lcd.print("                    ");
  lcd.setCursor(0, row);
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
  byte strikes = htcom.getStrikes();
  for (uint8_t x = 0; x < 3; x++)
  {
    pixel.setPixelColor(x, GREEN);
    if (x < strikes)
    {
      pixel.setPixelColor(x, RED);
      lcd.setCursor(10 + x, 0);
      lcd.print('*');
    }
  }
  pixel.show();
}

void printHeader(bool withVersion)
{
  lcd.setCursor(0, 0);
  lcd.print(F("HI-Tane "));
  if (withVersion)
    lcd.print(F(Version));

  strcpy_P(buffer, (char *)pgm_read_word(&(GAMEMODE_NAMES[difficulty])));
  lcd.print(buffer[0]);
}

void printWelcome()
{
  lcd.setCursor(0, 1);
  lcd.print(F("Welcome"));
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
  clearRow(0);
  printHeader(true);

  clearRow(1);
  clearRow(2);
  clearRow(3);
  lcd.setCursor(0, 1);
  lcd.print(F("Hurray, you have"));
  lcd.setCursor(0, 2);
  lcd.print(F("unarmed the bomb"));
  lcd.setCursor(0, 3);
  lcd.print(F("Another game? <yes>"));
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
  clearRow(0);
  printHeader(true);

  clearRow(1);
  clearRow(2);
  clearRow(3);
  lcd.setCursor(0, 1);
  lcd.print(F("sorry, you explodes!"));
  lcd.setCursor(0, 3);
  lcd.print(F("Another game? <yes>"));
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
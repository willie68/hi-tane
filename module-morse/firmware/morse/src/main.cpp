#include <Arduino.h>
#include <TM1637Display.h>
#include <avdweb_Switch.h>

#define debug
#include <debug.h>
#include "game.h"
#include <words.h>
#include <morse.h>

// ---- forward declarations
void initGame();
byte getSlider();
void poll();

// RGB LED
const uint8_t LED_PIN = 4;
// Game framework
Game game(ModuleTag::MORSE, LED_PIN);

const uint8_t MORSE_BEEP_PIN = 5;
const uint8_t SLIDER_PIN = 0; // this is a0
const uint8_t BUTTON_PIN = 3;

const uint8_t MORSE_LED_PIN = 7;
const uint8_t MORSE_LED_GND = 6;

const uint8_t CLK = 8;
const uint8_t DIO = 9;
// 7-Segment LED Display
const uint8_t TTD[] = {SEG_F | SEG_G | SEG_E | SEG_D};
const uint8_t MND[] = {SEG_G};

Switch btn = Switch(BUTTON_PIN); // Button north
TM1637Display display = TM1637Display(CLK, DIO);
Morse morse = Morse(MORSE_LED_PIN, MORSE_BEEP_PIN, 250);

byte sbr = 1;

void setup()
{
  //  initDebug();
  Serial.begin(115200);

  pinMode(MORSE_LED_PIN, OUTPUT);
  pinMode(MORSE_LED_GND, OUTPUT);
  pinMode(MORSE_BEEP_PIN, OUTPUT);

  game.init();

  initGame();

  display.clear();
  display.setBrightness(7);
}

WordFrequency wordFreq;

void initGame()
{
  randomSeed(analogRead(0));
  byte idx = random(WORD_COUNT);
  wordFreq = wordset[idx];
  dbgOut(F("word: "));
  dbgOut(wordFreq.word);
  dbgOut(F(", freq:"));
  dbgOutLn2(wordFreq.frq, DEC);
  switch (game.getGameDifficulty())
  {
  case Difficulty::SIMPLE:
  case Difficulty::NUM_DIFF:
    morse.setDitLength(400);
    break;
  case Difficulty::MEDIUM:
    morse.setDitLength(250);
    break;
  case Difficulty::HARD:
    morse.setDitLength(150);
    break;
  }
  dbgOut(F("set dit length: "));
  dbgOutLn(morse.getDitLength());
  game.arm();
  sbr = 0;
}

long freq, sfreq;
char buf[10];
unsigned long striketime = 0;

void loop()
{
  poll();

  if (game.isState(ModuleState::ARMED) && morse.finished())
  {
    dbgOutLn("new message");
    wordFreq.word.toCharArray(buf, 10);
    morse.sendMessage(buf);
  }

  if (striketime > millis())
  {
    return;
  }

  //  freq = map(getSlider(), 0, 128, frqBase, frqBase + 128);
  freq = getSlider() + frqBase;
  if (freq != sfreq)
  {
    dbgOut("freq: ");
    dbgOutLn2(freq, DEC);
    sfreq = freq;
    display.showNumberDec(freq);
  }
  if (btn.singleClick())
  {
    Serial.println("clicked");
    if (freq == wordFreq.frq)
    {
      dbgOutLn("solved");
      game.setSolved();
    }
    else
    {
      dbgOutLn("strike");
      game.setStrike();
      striketime = millis() + 5000;
      display.setSegments(MND, 1, 0);
      display.setSegments(MND, 1, 1);
      display.setSegments(MND, 1, 2);
      display.setSegments(MND, 1, 3);
    }
  }
}

const byte MAX = 100;
byte values[MAX];
byte getSlider()
{
  byte min = 255;
  byte max = 0;
  word sum = 0;
  for (byte i = 1; i < MAX; i++)
  {
    values[i - 1] = values[i];
    sum += values[i - 1];
    if (values[i - 1] < min)
    {
      min = values[i - 1];
    }
    if (values[i - 1] > max)
    {
      max = values[i - 1];
    }
  }
  values[MAX - 1] = byte(analogRead(SLIDER_PIN) >> 3);
  sum += values[MAX - 1];
  sum -= min;
  sum -= max;
  return sum / (MAX - 1);
}

void poll()
{
  game.poll();

  if (sbr != game.getBrightness())
  {
    sbr = game.getBrightness();
    analogWrite(MORSE_LED_GND, 255 - (sbr * 16));
    display.setBrightness(sbr >> 1);
  }

  if (game.isNewGameSettings())
  {
    initGame();
  }
  btn.poll();
  morse.poll();
}
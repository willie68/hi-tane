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

// RGB LED
#define LED_PIN 4
// Game framework
Game game(ModuleTag::MORSE, LED_PIN);

#define BEEP_PIN 5
#define SLIDER_PIN 0 // this is a0
#define BUTTON_PIN 7

#define MORSE_LED_PIN 6

#define CLK 8
#define DIO 9
// 7-Segment LED Display
const uint8_t TTD[] = {SEG_F | SEG_G | SEG_E | SEG_D};
const uint8_t MND[] = {SEG_G};

Switch btn = Switch(BUTTON_PIN); // Button north
TM1637Display display = TM1637Display(CLK, DIO);
Morse morse = Morse(MORSE_LED_PIN, BEEP_PIN, 250);

void setup()
{
  //  initDebug();
  Serial.begin(115200);

  pinMode(MORSE_LED_PIN, OUTPUT);

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
  Serial.print("word: ");
  Serial.print(wordFreq.word);
  Serial.print(", freq:");
  Serial.println(wordFreq.frq, DEC);
  game.arm();
}

long freq, sfreq;
char SOS[] = {'s', 'o', 's', 0x00};
char buf[10];
unsigned long striketime = 0;

void loop()
{
  game.poll();
  btn.poll();
  morse.poll();

  if (game.isState(ModuleState::ARMED)  && morse.finished())
  {
    dbgOutLn("new message");
    wordFreq.word.toCharArray(buf, 10);
    morse.sendMessage(buf);
  }

  if (striketime > millis())
  {
    return;
  }

  freq = map(getSlider(), 0, 255, frqBase, frqBase + 255);
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

byte getSlider()
{
  return byte(analogRead(SLIDER_PIN) >> 2);
}
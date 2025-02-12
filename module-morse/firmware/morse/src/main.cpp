#include <Arduino.h>
#include <TM1637Display.h>
#include <avdweb_Switch.h>

#define debug
#define wokwi
#include <debug.h>
#include "game.h"
#include <words.h>
#include <morse.h>

// ---- forward declarations
void initGame();
byte getSlider();

// RGB LED
#define LED_PIN 4
#define COM_PIN 11
// Game framework
Game game(ModuleTag::MORSE, LED_PIN, COM_PIN);

#define BEEP_PIN  12
#define SLIDER_PIN 0
#define BUTTON_PIN 10

#define MORSE_LED_PIN 9

#define CLK 2
#define DIO 3
// 7-Segment LED Display
const uint8_t TTD[] = {SEG_F | SEG_G | SEG_E | SEG_D};
const uint8_t MND[] = {SEG_G};

Switch btn = Switch(BUTTON_PIN);  // Button north
TM1637Display display = TM1637Display(CLK, DIO);
Morse morse = Morse(MORSE_LED_PIN, BEEP_PIN);

void setup()
{
  initDebug();
  Serial.begin(115200);

  pinMode(MORSE_LED_PIN, OUTPUT);
  //pinMode(BUTTON_PIN, INPUT_PULLUP);

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

void loop()
{
    game.poll();
    btn.poll();

    digitalWrite(MORSE_LED_PIN, 1);
    delay(100);
    digitalWrite(MORSE_LED_PIN, 0);
    delay(100);
    freq = map(getSlider(), 0, 255, frqBase, frqBase + 255);
    if (freq != sfreq) {
      sfreq = freq;
      display.showNumberDec(freq);
    }
    if (btn.singleClick()) {
      Serial.println("clicked");
      if (freq == wordFreq.frq) {
        game.setSolved();
      } else {
        game.setStrike();
      }
    }
}

byte getSlider() {
  return byte(analogRead(SLIDER_PIN) >> 2);
}
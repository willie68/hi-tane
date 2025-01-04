#include <Arduino.h>
#include <TM1637Display.h>
#include <Adafruit_NeoPixel.h>


// Display
#define CLK 2
#define DIO 3

// RGB LED
#define LED_PIN 4


const uint8_t TTD[] = {SEG_F | SEG_G | SEG_E | SEG_D};
const uint8_t MND[] = {SEG_G};

const int MAX_TIME = 3600;

TM1637Display display = TM1637Display(CLK, DIO);
Adafruit_NeoPixel pixel(3, LED_PIN, NEO_GRB + NEO_KHZ800);

const uint32_t RED = pixel.Color(0xff, 0, 0);
const uint32_t BLUE = pixel.Color(0, 0, 0xff);
const uint32_t GREEN = pixel.Color(0, 0xff, 0);

void showTime(int act);

uint32_t color = BLUE;
long start; 

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  display.clear();
  display.setSegments(TTD, 1, 0);
  display.setBrightness(7);

  pixel.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixel.setBrightness(7 * 8);
  pixel.show();

  start = millis();
  Serial.begin(115200);
}

void loop() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(500);
  int act = MAX_TIME - ((millis() - start) / 1000);
  showTime(act);
  Serial.println(act);
  pixel.setPixelColor(0, GREEN);
  pixel.setPixelColor(1, RED);
  pixel.setPixelColor(2, BLUE);
  pixel.show();
}

void showTime(int act)
{
  bool neg = act < 0;
  int t = abs(act);
  byte sec = t % 60;
  byte min = (t - sec) / 60;
  if (neg)
  {
    display.setSegments(MND, 1, 0);
    display.showNumberDec(min, false, 1, 1);
  }
  else
  {
    display.showNumberDec(min, false, 2, 0);
  }
  display.showNumberDec(sec, true, 2, 2);
}

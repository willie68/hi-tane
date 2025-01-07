#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define debug
#include "indicators.h"
#include "communication.h"
#include <panel.h>

// RGB LED
#define LED_PIN 4
Adafruit_NeoPixel pixel(3, LED_PIN, NEO_GRB + NEO_KHZ800);
const uint32_t PX_RED = pixel.Color(0xff, 0, 0);
const uint32_t PX_BLUE = pixel.Color(0, 0, 0xff);
const uint32_t PX_GREEN = pixel.Color(0, 0xff, 0);
const uint32_t PX_YELLOW = pixel.Color(0xff, 0xff, 0);

const byte COM_PIN = 11;
HTCOM htcom;

Panel panel;

// --- forward functions
void initGame();

void setup()
{
  Serial.begin(115200);
  Serial.println("init");
  pinMode(LED_BUILTIN, OUTPUT);

  pixel.setPixelColor(0, PX_BLUE);
  pixel.show();

  htcom = HTCOM();
  htcom.attach(COM_PIN, ID_WIRES);

  initGame();
}

void loop()
{
  // put your main code here, to run repeatedly:
}


void initGame()
{
  pixel.setPixelColor(0, PX_BLUE);
  pixel.show();


  bool invalid = true;
  while (invalid) {
    invalid = !panel.init(true); //TODO setting the sn from HTCOM
    if (invalid) {
      delay(1000);
      Serial.println(F("invalid wire configuration"));
      htcom.sendError("wires: invalid configuration");
    }
  }

  pixel.setPixelColor(0, PX_RED);
  pixel.show();
}

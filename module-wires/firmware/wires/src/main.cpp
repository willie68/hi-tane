#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define debug
#include "indicators.h"
#include "communication.h"
#include <panel.h>

// RGB LED
#define LED_PIN 4
Adafruit_NeoPixel pixel(3, LED_PIN, NEO_GRB + NEO_KHZ800);
const uint32_t PX_BLACK = pixel.Color(0, 0, 0);
const uint32_t PX_RED = pixel.Color(0xff, 0, 0);
const uint32_t PX_BLUE = pixel.Color(0, 0, 0xff);
const uint32_t PX_GREEN = pixel.Color(0, 0xff, 0);
const uint32_t PX_YELLOW = pixel.Color(0xff, 0xff, 0);

const byte COM_PIN = 11;
HTCOM htcom;

Panel panel;

enum MODULE_STATE
{
  INIT,
  ARMED,
  STRIKED,
  DISARMED
};

MODULE_STATE moduleState;

// --- forward functions
void initGame();

void setup()
{
  moduleState = INIT;
  Serial.begin(115200);
  Serial.println("init");
  pinMode(LED_BUILTIN, OUTPUT);

  pixel.setPixelColor(0, PX_BLUE);
  pixel.show();

  htcom = HTCOM();
  htcom.attach(COM_PIN, ID_WIRES);

  initGame();
  moduleState = ARMED;
}

void loop()
{
  if (moduleState == DISARMED)
  {
    delay(1000);
    pixel.setPixelColor(0, PX_GREEN);
  }
  else if (panel.isDisarmed() && moduleState == ARMED)
  {
    moduleState = DISARMED;
    htcom.sendDisarmed();
    pixel.setPixelColor(0, PX_GREEN);
  }
  else if (panel.isStriken() && moduleState == ARMED)
  {
    moduleState = STRIKED;
    htcom.sendStrike();
    pixel.setPixelColor(0, PX_RED);
  }
  else if ((moduleState == STRIKED) && !panel.isStriken())
  {
    moduleState = ARMED;
    pixel.setPixelColor(0, PX_RED);
  }

  if (moduleState == STRIKED)
  {
    if ((millis() % 250) < 125)
    {
      pixel.setPixelColor(0, PX_RED);
    }
    else
    {
      pixel.setPixelColor(0, PX_BLACK);
    }
  }
  pixel.show();
}

void initGame()
{
  pixel.setPixelColor(0, PX_BLUE);
  pixel.show();

  bool invalid = true;
  while (invalid)
  {
    invalid = !panel.init(true); // TODO setting the sn from HTCOM
    if (invalid)
    {
      delay(1000);
      Serial.println(F("invalid wire configuration"));
      htcom.sendError("wires: invalid configuration");
    }
  }

  pixel.setPixelColor(0, PX_RED);
  pixel.show();
}

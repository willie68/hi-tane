#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define debug
#include "indicators.h"
#include "communication.h"
#include <panel.h>

// ---- forward declarations
void showTime(int act);

// RGB LED
#define LED_PIN 4
Adafruit_NeoPixel pixel(1, LED_PIN, NEO_RGB + NEO_KHZ800);
const uint32_t PX_BLACK = pixel.Color(0, 0, 0);
const uint32_t PX_RED = pixel.Color(0xff, 0, 0);
const uint32_t PX_BLUE = pixel.Color(0, 0, 0xff);
const uint32_t PX_GREEN = pixel.Color(0, 0xff, 0);
const uint32_t PX_YELLOW = pixel.Color(0xff, 0xff, 0);
const uint32_t PX_WHITE = pixel.Color(0xff, 0xff, 0xff);
const uint32_t PX_BROWN = pixel.Color(0x5b, 0x3a, 0x29);

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
  pinMode(LED_PIN, OUTPUT);

  pixel.setPixelColor(0, PX_BLUE);
  pixel.show();

  htcom = HTCOM();
  htcom.attach(COM_PIN, ID_WIRES);

  initGame();
  moduleState = ARMED;
}

MODULE_STATE saveState = MODULE_STATE::INIT;

void loop()
{

  htcom.poll();
  if (saveState != moduleState)
  {
    Serial.print(F("Module state:"));
    Serial.println(moduleState);
    saveState = moduleState;
  }

  if (moduleState == DISARMED)
  {
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
    Serial.print(F("strike "));
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

  showTime(htcom.getGameTime());
}

void initGame()
{
  pixel.setPixelColor(0, PX_BLUE);
  pixel.show();

  bool invalid = true;
  while (invalid)
  {
    invalid = !panel.init(true); // TODO setting the sn from HTCOM
    invalid = false;
    if (invalid)
    {
      delay(1000);
      Serial.println(F("invalid wire configuration"));
      htcom.sendError("wires: invalid configuration");
    }
  }

  pixel.setPixelColor(0, PX_RED);
  pixel.show();
  panel.printPlugs();
}

int saveTime = 0;
void showTime(int act)
{
  if (act != saveTime)
  {
    saveTime = act;
    bool neg = act < 0;
    int t = abs(act);
    byte sec = t % 60;
    byte min = (t - sec) / 60;
    if (neg)
    {
      Serial.print("-");
      if (min <= 0)
        Serial.print("0");
      Serial.print(min);
    }
    else
    {
      if (min <= 0)
        Serial.print("0");
      Serial.print(min);
    }
    Serial.print(":");
    if (sec <= 0)
      Serial.print("0");
    Serial.println(sec);
  }
}
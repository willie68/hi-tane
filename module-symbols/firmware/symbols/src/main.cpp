/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using SPI to communicate
 4 or 5 pins are required to interface.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/
#define SSD1306_NO_SPLASH

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avdweb_Switch.h>
#include <symbols.h>
#define debug
#include <debug.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Switch btn0 = Switch(5);
Switch btn1 = Switch(6);
Switch btn2 = Switch(7);
Switch btn3 = Switch(8);

// Forward declarations
void initGame();
void poll();

void setup()
{
  initDebug();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  dbgOut("symbols: ");
  dbgOutLn(NUM_SYMBOLS);

  randomSeed(analogRead(0));

  initGame();
}

void doSegments()
{
  display.clearDisplay();
  display.drawFastVLine(32, 0, 32, SSD1306_WHITE);
  display.drawFastVLine(64, 0, 32, SSD1306_WHITE);
  display.drawFastVLine(96, 0, 32, SSD1306_WHITE);
  display.drawFastHLine(0, 0, 127, SSD1306_WHITE);
  display.drawFastHLine(0, 31, 127, SSD1306_WHITE);
}

void invert(int16_t x, int16_t y, int16_t w, int16_t h)
{
  display.fillRect(x, y, w, h, SSD1306_INVERSE);
  /*
  for (int16_t py = y; py < (y + h); py++)
  {
    for (int16_t px = x; px < (x + w); px++)
    {
      display.drawPixel(px, py, SSD1306_INVERSE);
    }
  }
  */
}

uint8_t col = 0;
uint8_t selected[4];
void initGame()
{
  // select a col
  col = random(0, 5);
  for (uint8_t x = 0; x < 4; x++)
  {
    bool found = false;
    while (!found)
    {
      selected[x] = random(7);
      found = true;
      for (uint8_t y = 0; y < x; y++)
      {
        if (selected[y] == selected[x])
        {
          found = false;
        }
      }
    }
  }
  dbgOut(F("selected col: "));
  dbgOut(col);
  dbgOut(F(": ")) for (uint8_t x = 0; x < 4; x++)
  {
    dbgOut(selected[x]);
  }
  dbgOutLn();
}

uint8_t pos = 0;
uint8_t spos = 255;
uint8_t scol = 255;
bool changed = true;

void loop()
{
  poll();
  if (btn0.singleClick())
  {
    invert(0, 0, 32, 32);
    display.display();
  }
  if (btn1.singleClick())
  {
    invert(32, 0, 32, 32);
    display.display();
  }
  if (btn2.singleClick())
  {
    invert(64, 0, 32, 32);
    display.display();
  }
  if (btn3.singleClick())
  {
    invert(96, 0, 32, 32);
    display.display();
  }

  if (changed)
  {
    changed = false;
    display.clearDisplay();
    for (uint8_t x = 0; x < 4; x++)
    {
      uint8_t row = selected[x];
      uint8_t symb = SYM_TABLE[col][row];
      dbgOut(F(", "));
      dbgOut(symb);
      display.drawBitmap(x * 32, 0, symbols[symb], 32, 32, SSD1306_WHITE);
    }
    dbgOutLn();
    display.display();
  }
}

void poll()
{
  btn0.poll();
  btn1.poll();
  btn2.poll();
  btn3.poll();
}
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
#include <symbols.h>

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

// Forward declarations
void testdrawline();

void testdrawrect(void);
void testfillrect(void);
void testdrawcircle(void);
void testfillcircle(void);
void testdrawroundrect(void);
void testfillroundrect(void);
void testdrawtriangle(void);
void testfilltriangle(void);
void testdrawchar(void);
void testdrawstyles(void);
void testscrolltext(void);
void testdrawbitmap(void);
void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h);

#define NUMFLAKES 10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

static const unsigned char PROGMEM logo_bmp[] =
    {0b00000000, 0b11000000,
     0b00000001, 0b11000000,
     0b00000001, 0b11000000,
     0b00000011, 0b11100000,
     0b11110011, 0b11100000,
     0b11111110, 0b11111000,
     0b01111110, 0b11111111,
     0b00110011, 0b10011111,
     0b00011111, 0b11111100,
     0b00001101, 0b01110000,
     0b00011011, 0b10100000,
     0b00111111, 0b11100000,
     0b00111111, 0b11110000,
     0b01111100, 0b11110000,
     0b01110000, 0b01110000,
     0b00000000, 0b00110000};

void setup()
{
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
}
void doSegments()
{
  display.clearDisplay();
  display.drawFastVLine(32, 0, 32, SSD1306_WHITE);
  display.drawFastVLine(64, 0, 32, SSD1306_WHITE);
  display.drawFastVLine(96, 0, 32, SSD1306_WHITE);
}
void loop()
{
  for (uint8_t y = 0 ; y< 8; y++) {
    doSegments();
    for (uint8_t x = 0; x < 4; x++) {
      uint8_t idx = y*4 + x;
      if (x < sizeof(symbols)) {
        display.drawBitmap(x*32, 0, symbols[idx], 32, 32, SSD1306_WHITE);
      }
    }
    display.display();
    delay(2000);
  }
/*
doSegments();
display.drawBitmap(0, 0, g0005, 32, 32, SSD1306_WHITE);
  display.drawBitmap(32, 0, g0006, 32, 32, SSD1306_WHITE);
  display.drawBitmap(64, 0, g0007, 32, 32, SSD1306_WHITE);
  display.drawBitmap(96, 0, g0008, 32, 32, SSD1306_WHITE);

  display.display();
  delay(2000);
  
  doSegments();
  display.drawBitmap(0, 0, g0009, 32, 32, SSD1306_WHITE);
  display.drawBitmap(32, 0, g0010, 32, 32, SSD1306_WHITE);
  display.drawBitmap(64, 0, g0011, 32, 32, SSD1306_WHITE);
  display.drawBitmap(96, 0, g0012, 32, 32, SSD1306_WHITE);

  display.display();
  delay(2000);

  doSegments();
  display.drawBitmap(0, 0, g0013, 32, 32, SSD1306_WHITE);
  display.drawBitmap(32, 0, g0014, 32, 32, SSD1306_WHITE);
  display.drawBitmap(64, 0, g0015, 32, 32, SSD1306_WHITE);
  display.drawBitmap(96, 0, g0016, 32, 32, SSD1306_WHITE);
  
  display.display();
  delay(2000);

  doSegments();
  display.drawBitmap(0, 0, g0017, 32, 32, SSD1306_WHITE);
  display.drawBitmap(32, 0, g0018, 32, 32, SSD1306_WHITE);
  display.drawBitmap(64, 0, g0019, 32, 32, SSD1306_WHITE);
  display.drawBitmap(96, 0, g0020, 32, 32, SSD1306_WHITE);

  display.display();
  delay(2000);

  doSegments();
  display.drawBitmap(0, 0, g0021, 32, 32, SSD1306_WHITE);
  display.drawBitmap(32, 0, g0022, 32, 32, SSD1306_WHITE);
  display.drawBitmap(64, 0, g0023, 32, 32, SSD1306_WHITE);
  display.drawBitmap(96, 0, g0024, 32, 32, SSD1306_WHITE);
  
  display.display();
  delay(2000);
  
  doSegments();
  display.drawBitmap(0, 0, g0025, 32, 32, SSD1306_WHITE);
  display.drawBitmap(32, 0, g0026, 32, 32, SSD1306_WHITE);
  display.drawBitmap(64, 0, g0027, 32, 32, SSD1306_WHITE);

  display.display();
  delay(2000);
  */
}

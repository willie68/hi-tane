#include <Arduino.h>
#include <U8g2lib.h>

#define debug
#include <debug.h>
#include <game.h>
#include <serialnumber.h>

// ---- forward declarations
void initDisplay();
void initGame();
void updateInput();
void poll();
bool answerCorrect();
void showEffekt(bool solved);

// RGB LED
const byte LED_PIN = 4;
// Game framework
Game game(ModuleTag::PASSWORD, LED_PIN);

U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* clock=A5*/ 19, /* data=A4*/ 18);

byte cursor;
bool solved;

void setup()
{
  Serial.begin(115200);
  initDisplay();

  game.init();

  initGame();
  game.setSolved();
}

void initDisplay()
{
  randomSeed(analogRead(0));
  dbgOutLn("init display");
  u8x8.begin();
  // u8x8.setFont(u8x8_font_courB18_2x3_f);
  u8x8.setFont(u8x8_font_8x13_1x2_f);
  u8x8.clearDisplay();
  u8x8.setPowerSave(1);
  delay(1000);
  u8x8.setPowerSave(0);
}

SerialNumber serialNumber;
Indicators indicators;
bool changed;
char buffer[30];

void initGame()
{
  game.setTestParameter();

  uint32_t snr = game.getSerialNumber();
  serialNumber.Set(snr);

  indicators = game.getIndicators();

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

  changed = true;
}

void loop()
{
  poll();
  if (changed)
  {
    changed = false;
    serialNumber.String(buffer);
    u8x8.clearDisplay();
    u8x8.drawString(0, 0, "SNR: ");
    u8x8.drawString(5, 0, buffer);
    u8x8.drawString(0, 2, "TAG: ");
    u8x8.setCursor(5, 2);
    for (byte x = 0; x < indicators.Count(); x++)
    {
      if (x > 0)
      {
        u8x8.print(" ");
      }
      byte idx = indicators.Get(x);
      strcpy_P(buffer, (char *)pgm_read_word(&(INDICATORNAMES[idx])));
      if (indicators.IsActive(x))
      {
        u8x8.inverse();
      }
      u8x8.print(buffer);
      u8x8.noInverse();
    }
  }
}

void poll()
{
  game.poll();
  if (game.isNewGameSettings())
  {
    initGame();
    uint32_t snr = game.getSerialNumber();
    serialNumber.Set(snr);
    changed = true;
  }
}

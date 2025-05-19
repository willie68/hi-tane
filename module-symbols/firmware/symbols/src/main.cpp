#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#define SSD1306_NO_SPLASH
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avdweb_Switch.h>
#include <symbols.h>
#define debug
#include <debug.h>
#include <game.h>

// Game framework
#define LED_PIN 4
Game game(ModuleTag::SYMBOLS, LED_PIN);

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 

#define OLED_RESET -1       
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Switch btn0 = Switch(5);
Switch btn1 = Switch(6);
Switch btn2 = Switch(7);
Switch btn3 = Switch(8);

// Forward declarations
void initGame();
void invert(uint8_t idx);
void poll();
void resetClicked();
void drawSymbols();
void showSolveEffekt();

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

  game.init();

  initGame();
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

  resetClicked();
  drawSymbols();
  game.arm();
}

void drawSymbols()
{
  dbgOut(F("draw symbol: "));
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

uint8_t pos = 0;
uint8_t spos = 255;
uint8_t scol = 255;
bool changed = true;
bool clicked[4];

void checkBtnClick(uint8_t btn)
{
  bool ok = true;
  if (clicked[btn]) {
    invert(btn);
    clicked[btn] = true;
    display.display();
    return ;
  }
  for (uint8_t x = 0; x < 4; x++)
  {
    if (x != btn)
    {
      if (!clicked[x])
      {
        uint8_t row = selected[x];
        if (row < selected[btn])
        {
          ok = false;
          break;
        }
      }
    }
  }
  if (ok)
  {
    invert(btn);
    clicked[btn] = true;
    display.display();
  }
  else
  {
    dbgOutLn(F("strike"));
    resetClicked();
    drawSymbols();
    game.setStrike();
  }
}

void loop()
{
  poll();
  if (game.isState(ModuleState::DISARMED))
    showSolveEffekt();

  if (btn0.singleClick())
  {
    checkBtnClick(0);
  }
  if (btn1.singleClick())
  {
    checkBtnClick(1);
  }
  if (btn2.singleClick())
  {
    checkBtnClick(2);
  }
  if (btn3.singleClick())
  {
    checkBtnClick(3);
  }
  bool allClicked = true;
  for (uint8_t x = 0; x < 4; x++)
  {
    allClicked = allClicked && clicked[x];
  }
  if (allClicked)
  {
    if (game.isState(ModuleState::ARMED)) {
      dbgOutLn(F("solved"));
      display.clearDisplay();
      display.setTextSize(2); // Draw 2X-scale text
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(30, 0);
      display.println(F("solved"));
      display.display();
      game.setSolved();
    }
  }
}

void poll()
{
  game.poll();
  if (game.isNewGameSettings())
  {
    initGame();
  }

  btn0.poll();
  btn1.poll();
  btn2.poll();
  btn3.poll();
}

void resetClicked()
{
  for (uint8_t x = 0; x < 4; x++)
    clicked[x] = false;
}

void invert(uint8_t idx)
{
  display.fillRect(idx * 32, 0, 32, 32, SSD1306_INVERSE);
}

void showSolveEffekt() {

}
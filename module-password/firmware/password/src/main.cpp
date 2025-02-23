#include <Arduino.h>
#include <passwords.h>
#include <U8g2lib.h>
#include <avdweb_Switch.h>

#define debug
#include <debug.h>
#include "game.h"

// ---- forward declarations
void initGame();
void updateInput();
void poll();
bool answerCorrect();
void showSolveEffekt();
void showStrikeEffekt();

// RGB LED
#define LED_PIN 4
// Game framework
Game game(ModuleTag::PASSWORD, LED_PIN);

U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* clock=A5*/ 19, /* data=A4*/ 18);

Switch btsubmit = Switch(5); // Button north
Switch bt0 = Switch(6);      // Button north
Switch bt1 = Switch(7);      // Button east
Switch bt2 = Switch(8);      // Button south
Switch bt3 = Switch(9);      // Button west
Switch bt4 = Switch(3);      // Button middle

char answer[6];
char alpha0[7];
char alpha1[7];
char alpha2[7];
char alpha3[7];
char alpha4[7];
char input[6];
char curline[6];
byte xo[5];
byte cursor;
bool solved;

void setup()
{
  Serial.begin(115200);
  u8x8.begin();
  //  u8x8.setFont(u8x8_font_inr21_2x4_f);
  u8x8.setFont(u8x8_font_profont29_2x3_f);
  u8x8.clearDisplay();
  u8x8.setPowerSave(1);
  delay(1000);
  u8x8.setPowerSave(0);

  randomSeed(analogRead(0));
  game.init();

  initGame();

  game.arm();
}

void initGame()
{
  byte wd = random(PWD_COUNT);
  dbgOutLn("init game");
  // Difficulty dif = game.getGameDifficulty();
  byte off = wd * 6;
  strcpy_P(answer, (char *)pgm_read_word(&(pwd[off + 0])));
  strcpy_P(alpha0, (char *)pgm_read_word(&(pwd[off + 1])));
  strcpy_P(alpha1, (char *)pgm_read_word(&(pwd[off + 2])));
  strcpy_P(alpha2, (char *)pgm_read_word(&(pwd[off + 3])));
  strcpy_P(alpha3, (char *)pgm_read_word(&(pwd[off + 4])));
  strcpy_P(alpha4, (char *)pgm_read_word(&(pwd[off + 5])));

  for (byte i = 0; i < 5; i++)
  {
    xo[i] = random(5);
  }

  cursor = 0;
  updateInput();

  Serial.println(answer);
  Serial.println(alpha0);
  Serial.println(alpha1);
  Serial.println(alpha2);
  Serial.println(alpha3);
  Serial.println(alpha4);
  Serial.println(input);

  solved = false;
}

bool changed;
void loop()
{
  poll();
  if (solved)
  {
    return;
  }
  if (game.isState(ModuleState::DISARMED) && !solved)
  {
    solved = true;
    showSolveEffekt();
    return;
  }

  if (bt0.singleClick())
  {
    cursor++;
    cursor = cursor % 5;
    changed = true;
  }
  if (bt1.singleClick())
  {
    xo[cursor]++;
    xo[cursor] = xo[cursor] % 6;
    updateInput();
  }
  if (btsubmit.singleClick())
  {
    if (answerCorrect())
    {
      game.setSolved();
      input[0] = '-';
      input[1] = '-';
      input[2] = '-';
      input[3] = '-';
      input[4] = '-';
      changed = true;
    }
    else
    {
      game.setStrike();
      changed = true;
    }
  }
  if (changed)
  {
    changed = false;
    if (game.isState(ModuleState::STRIKED))
    {
      showStrikeEffekt();
      for (byte i = 0; i < 60; i++)
      {
        game.poll();
        delay(50);
      }
      u8x8.drawString(2, 1, "      ");
      game.arm();
    }
    if (game.isState(ModuleState::DISARMED))
    {
      showSolveEffekt();
    }
    else
    {
      for (byte i = 0; i < 5; i++)
      {
        if (i == cursor)
        {
          curline[i] = '_';
        }
        else
        {
          curline[i] = ' ';
        }
      }
      curline[5] = 0x00;
      u8x8.drawString(3, 1, curline);
      u8x8.drawString(3, 0, input);
    }

    Serial.println(input);
  }
}

void showSolveEffekt()
{
  u8x8.drawString(2, 0, "      ");
  u8x8.drawString(2, 1, "SOLVED");
}

void showStrikeEffekt()
{
  u8x8.drawString(2, 0, "      ");
  u8x8.drawString(2, 1, "STRIKE");
}

void updateInput()
{
  input[0] = alpha0[xo[0]];
  input[1] = alpha1[xo[1]];
  input[2] = alpha2[xo[2]];
  input[3] = alpha3[xo[3]];
  input[4] = alpha4[xo[4]];
  input[5] = 0x00;

  changed = true;
}

bool answerCorrect()
{
  for (byte i = 0; i < 5; i++)
  {
    if (input[i] != answer[i])
      return false;
  }
  return true;
}

void poll()
{
  game.poll();
  if (game.isNewGameSettings())
  {
    initGame();
  }

  btsubmit.poll();
  bt0.poll();
  bt1.poll();
  bt2.poll();
  bt3.poll();
  bt4.poll();
}
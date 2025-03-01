#include <Arduino.h>
#include <passwords.h>
#include <passwords6.h>
#include <U8g2lib.h>
#include <avdweb_Switch.h>

#define debug
#include <debug.h>
#include "game.h"

// ---- forward declarations
void initDisplay();
void initGame();
void updateInput();
void poll();
bool answerCorrect();
void showEffekt(bool solved);

// RGB LED
#define LED_PIN 4
// Game framework
Game game(ModuleTag::PASSWORD, LED_PIN);

U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* clock=A5*/ 19, /* data=A4*/ 18);

Switch btsubmit = Switch(5); // submit the worf
Switch bt0 = Switch(6);      // switch the cursor
Switch bt1 = Switch(7);      // scroll through letters

char answer[7];
char alpha0[7];
char alpha1[7];
char alpha2[7];
char alpha3[7];
char alpha4[7];
char alpha5[7];
char input[7];
char curline[7];
byte xo[6];
byte cursor;
bool solved;
byte positions; // the count of possible positions (e.g. number of letters in the password)
bool hard;

void setup()
{
  Serial.begin(115200);
  initDisplay();

  randomSeed(analogRead(0));
  game.init();

  initGame();

  game.arm();
}

void initDisplay()
{
  dbgOutLn("init display");
  u8x8.begin();
  // u8x8.setFont(u8x8_font_courB18_2x3_f);
  u8x8.setFont(u8x8_font_profont29_2x3_r);
  u8x8.clearDisplay();
  u8x8.setPowerSave(1);
  delay(1000);
  u8x8.setPowerSave(0);
  u8x8.setFlipMode(1);
}

void initGame()
{
  // game.setGameDifficulty(Difficulty::HARD);
  byte wd = random(PWD_COUNT);
  dbgOutLn("init game");
  dbgOut("dif:");
  dbgOut(game.getGameDifficulty());
  dbgOut(" ");
  if (game.isGameDifficulty(Difficulty::HARD))
  {
    dbgOutLn("HARD");
    positions = 6;
    hard = true;
    byte off = wd * 7;
    strcpy_P(answer, (char *)pgm_read_word(&(pwd6[off + 0])));
    strcpy_P(alpha0, (char *)pgm_read_word(&(pwd6[off + 1])));
    strcpy_P(alpha1, (char *)pgm_read_word(&(pwd6[off + 2])));
    strcpy_P(alpha2, (char *)pgm_read_word(&(pwd6[off + 3])));
    strcpy_P(alpha3, (char *)pgm_read_word(&(pwd6[off + 4])));
    strcpy_P(alpha4, (char *)pgm_read_word(&(pwd6[off + 5])));
    strcpy_P(alpha5, (char *)pgm_read_word(&(pwd6[off + 6])));
  }
  else
  {
    dbgOutLn("SIM/MED");
    positions = 5;
    hard = false;
    byte off = wd * 6;
    strcpy_P(answer, (char *)pgm_read_word(&(pwd[off + 0])));
    strcpy_P(alpha0, (char *)pgm_read_word(&(pwd[off + 1])));
    strcpy_P(alpha1, (char *)pgm_read_word(&(pwd[off + 2])));
    strcpy_P(alpha2, (char *)pgm_read_word(&(pwd[off + 3])));
    strcpy_P(alpha3, (char *)pgm_read_word(&(pwd[off + 4])));
    strcpy_P(alpha4, (char *)pgm_read_word(&(pwd[off + 5])));
  }

  for (byte i = 0; i < positions; i++)
    xo[i] = random(5);

  cursor = 0;
  updateInput();

  dbgOutLn(answer);
  dbgOutLn(alpha0);
  dbgOutLn(alpha1);
  dbgOutLn(alpha2);
  dbgOutLn(alpha3);
  dbgOutLn(alpha4);
  if (hard)
    dbgOutLn(alpha5);
  dbgOutLn(input);

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
    showEffekt(true);
    return;
  }

  if (bt0.singleClick())
  {
    cursor++;
    cursor = cursor % positions;
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
      input[5] = '-';
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
      showEffekt(false);
      for (byte i = 0; i < 60; i++)
      {
        game.poll();
        delay(50);
      }
      u8x8.drawString(2, 1, "       ");
      game.arm();
    }
    if (game.isState(ModuleState::DISARMED))
    {
      showEffekt(true);
    }
    else
    {
      for (byte i = 0; i < positions; i++)
      {
        curline[i] = ' ';
        if (i == cursor)
          curline[i] = '_';
      }
      curline[positions] = 0x00;
      byte idx = 3;
      if (hard)
        idx = 2;

      u8x8.drawString(idx, 1, curline);
      u8x8.drawString(idx, 0, input);
    }

    dbgOutLn(input);
  }
}

void showEffekt(bool solved)
{
  u8x8.drawString(2, 0, "        ");
  if (solved)
  {
    u8x8.drawString(2, 1, "SOLVED");
  }
  else
  {
    u8x8.drawString(2, 1, "STRIKE");
  }
}

void updateInput()
{
  input[0] = alpha0[xo[0]];
  input[1] = alpha1[xo[1]];
  input[2] = alpha2[xo[2]];
  input[3] = alpha3[xo[3]];
  input[4] = alpha4[xo[4]];
  if (hard)
  {
    input[5] = alpha5[xo[5]];
    input[6] = 0x00;
  }
  else
  {
    input[5] = 0x00;
  }

  changed = true;
}

bool answerCorrect()
{
  for (byte i = 0; i < positions; i++)
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
}

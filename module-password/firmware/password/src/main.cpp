#include <Arduino.h>
#include <passwords.h>
#include <U8g2lib.h>
#include <avdweb_Switch.h>

#define debug
#define wokwi
#include "game.h"

// ---- forward declarations
void initGame(byte wd);
void updateInput();
void btnpoll();
bool answerCorrect();

// RGB LED
#define LED_PIN 4
#define COM_PIN 11
// Game framework
Game game(ModuleTag::PASSWORD, LED_PIN, COM_PIN);

// U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8x8(U8G2_R0); ///* clock=A5*/ 19, /* data=A4*/ 18);
// U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8x8(U8G2_R0, 255, 19, 18); ///* clock=A5*/ 19, /* data=A4*/ 18);
//  #include <U8x8lib.h>
U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=A5*/ 19, /* data=A4*/ 18);

Switch btsubmit = Switch(5); // Button north
Switch bt0 = Switch(6);      // Button north
Switch bt1 = Switch(7);      // Button east
Switch bt2 = Switch(8);      // Button south
Switch bt3 = Switch(9);      // Button west
Switch bt4 = Switch(10);     // Button middle

char answer[6];
char alpha0[7];
char alpha1[7];
char alpha2[7];
char alpha3[7];
char alpha4[7];
char input[6];
byte x0, x1, x2, x3, x4 = 0;

void setup()
{
  Serial.begin(115200);
  u8x8.begin();
  u8x8.setFont(u8x8_font_courB18_2x3_f);
  u8x8.clearDisplay();
  u8x8.setPowerSave(1);
  delay(1000);
  u8x8.setPowerSave(0);

  randomSeed(analogRead(0));
  byte idx = random(PWD_COUNT);
  initGame(idx);
}

void initGame(byte wd)
{
  game.init();
  byte off = wd * 6;
  strcpy_P(answer, (char *)pgm_read_word(&(pwd[off + 0])));
  strcpy_P(alpha0, (char *)pgm_read_word(&(pwd[off + 1])));
  strcpy_P(alpha1, (char *)pgm_read_word(&(pwd[off + 2])));
  strcpy_P(alpha2, (char *)pgm_read_word(&(pwd[off + 3])));
  strcpy_P(alpha3, (char *)pgm_read_word(&(pwd[off + 4])));
  strcpy_P(alpha4, (char *)pgm_read_word(&(pwd[off + 5])));

  updateInput();

  Serial.println(answer);
  Serial.println(alpha0);
  Serial.println(alpha1);
  Serial.println(alpha2);
  Serial.println(alpha3);
  Serial.println(alpha4);
  Serial.println(input);
  game.arm();
}

bool changed;
void loop()
{
  game.poll();
  btnpoll();
  if (bt0.singleClick())
  {
    x0++;
    x0 = x0 % 6;
    updateInput();
  }
  if (bt1.singleClick())
  {
    x1++;
    x1 = x1 % 6;
    updateInput();
  }
  if (bt2.singleClick())
  {
    x2++;
    x2 = x2 % 6;
    updateInput();
  }
  if (bt3.singleClick())
  {
    x3++;
    x3 = x3 % 6;
    updateInput();
  }
  if (bt4.singleClick())
  {
    x4++;
    x4 = x4 % 6;
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
    }
  }
  if (changed)
  {
    changed = false;
    u8x8.clearDisplay();
    u8x8.setPowerSave(1);
    delay(1000);
    u8x8.setPowerSave(0);
    if (game.isState(ModuleState::STRIKED))
    {
      u8x8.drawString(0, 0, "STRIKE");
      for (byte i = 0; i < 60; i++)
      {
        game.poll();
        delay(50);
      }
      game.arm();
    }
    if (game.isState(ModuleState::DISARMED))
    {
      u8x8.drawString(0, 0, "SOLVED");
    }
    else
    {
      u8x8.drawString(0, 0, input);
    }

    Serial.println(input);
  }
}

void updateInput()
{
  input[0] = alpha0[x0];
  input[1] = alpha1[x1];
  input[2] = alpha2[x2];
  input[3] = alpha3[x3];
  input[4] = alpha4[x4];
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

void btnpoll()
{
  btsubmit.poll();
  bt0.poll();
  bt1.poll();
  bt2.poll();
  bt3.poll();
  bt4.poll();
}
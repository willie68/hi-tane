#include <Arduino.h>
#include <passwords.h>
#include <U8x8lib.h>

U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=A5*/ 19, /* data=A4*/ 18);

char answer[6];
char alpha0[7];
char alpha1[7];
char alpha2[7];
char alpha3[7];
char alpha4[7];

void setup()
{
  Serial.begin(115200);
  u8x8.begin();
  u8x8.setPowerSave(0);
}

void initGame(byte wd)
{
  byte off = wd * 6;
  strcpy_P(answer, (char *)pgm_read_word(&(pwd[off + 0])));
  strcpy_P(alpha0, (char *)pgm_read_word(&(pwd[off + 1])));
  strcpy_P(alpha1, (char *)pgm_read_word(&(pwd[off + 2])));
  strcpy_P(alpha2, (char *)pgm_read_word(&(pwd[off + 3])));
  strcpy_P(alpha3, (char *)pgm_read_word(&(pwd[off + 4])));
  strcpy_P(alpha4, (char *)pgm_read_word(&(pwd[off + 5])));
}

long count = 0;

void loop()
{
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
  uint8_t rows = u8x8.getRows();
  uint8_t cols = u8x8.getCols();
  char rowsArr[3];
  char colArr[3];
  String(rows).toCharArray(rowsArr, 4);
  String(cols).toCharArray(colArr, 4);
  u8x8.draw2x2String(0, 0, "rows:");
  u8x8.draw2x2String(12, 0, rowsArr);
  u8x8.drawString(0, 2, "cols:");
  u8x8.draw2x2String(8, 2, colArr);
  u8x8.drawString(0, 4, "äöüß");
  delay(5000);


  for (byte count = 0; count < 35; count++)
  {
    initGame(count);
    u8x8.clearDisplay();
    u8x8.setPowerSave(1);
    delay(1000);
    u8x8.setPowerSave(0);

    u8x8.drawString(0, 0, answer);
    u8x8.draw2x2String(0, 2, alpha0);
    u8x8.draw2x2String(8, 3, alpha1);
    u8x8.draw2x2String(0, 4, alpha2);
    u8x8.draw2x2String(0, 4, alpha3);
    u8x8.drawString(0, 6, alpha4);
    Serial.println(answer);
    Serial.println(alpha0);
    Serial.println(alpha1);
    Serial.println(alpha2);
    Serial.println(alpha3);
    Serial.println(alpha4);
    Serial.println("------------");
    delay(5000);
  }
}

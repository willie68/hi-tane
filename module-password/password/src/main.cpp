#include <Arduino.h>
#include <passwords.h>

char answer[6];
char alpha0[7];
char alpha1[7];
char alpha2[7];
char alpha3[7];
char alpha4[7];

void setup()
{
  Serial.begin(115200);
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
  for (byte count = 0; count < 35; count++)
  {
    initGame(count);
    Serial.println(answer);
    Serial.println(alpha0);
    Serial.println(alpha1);
    Serial.println(alpha2);
    Serial.println(alpha3);
    Serial.println(alpha4);
    Serial.println("------------");
    delay(1000);
  }
}

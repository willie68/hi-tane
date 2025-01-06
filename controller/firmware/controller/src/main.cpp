#include <Arduino.h>
#include <TM1637Display.h>
#include <Adafruit_NeoPixel.h>
#include <ClickEncoder.h>
#include <timerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define debug
#include "indicators.h"
#include "communication.h"

// Display
#define CLK 2
#define DIO 3

// RGB LED
#define LED_PIN 4

// 7-Segment LED Display
const uint8_t TTD[] = {SEG_F | SEG_G | SEG_E | SEG_D};
const uint8_t MND[] = {SEG_G};

TM1637Display display = TM1637Display(CLK, DIO);

const int MAX_TIME = 3600;

// Encoder
const uint8_t PIN_ENCA = 6;
const uint8_t PIN_ENCB = 7;
const uint8_t PIN_BTN = 5;
const uint8_t ENC_STEPSPERNOTCH = 4;
const bool BTN_ACTIVESTATE = LOW;
const uint16_t TIMER_NOTIFY_US = 1000;
constexpr uint8_t PRINT_BASE = 10;
static TimerOne timer;
ClickEncoder clickEnc{PIN_ENCA, PIN_ENCB, PIN_BTN, ENC_STEPSPERNOTCH, BTN_ACTIVESTATE};

// LCD definitions
const uint8_t COLUMS = 20;
const uint8_t ROWS = 4;
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

// RGB LED
Adafruit_NeoPixel pixel(3, LED_PIN, NEO_GRB + NEO_KHZ800);
const uint32_t RED = pixel.Color(0xff, 0, 0);
const uint32_t BLUE = pixel.Color(0, 0, 0xff);
const uint32_t GREEN = pixel.Color(0, 0xff, 0);

// Tone
const uint8_t BEEP_PIN = 12;

const uint8_t COM_PIN = 11;
HTCOM htcom;

// --- forward-declared function prototypes:
void showTime(int act);
// Prints out button state
void printClickEncoderButtonState();
// Prints turn information (turn status, direction, Acceleration value)
void printClickEncoderValue();
// Prints accumulated turn information
void printClickEncoderCount();
void clearRow(uint8_t row);
void beep();
void dblBeep();
void generateSerialNumber();
void generateIndicators();
void printStatusLine();
void initGame();
void resetStrikes();
void showStrikes();

serial_t serialNumber = {'E', 'L', '5', 'E', 'R', '7'};
uint8_t indicators[3];
uint32_t color = BLUE;
long start;
char buffer[30];
bool strikes[3];

void timerIsr()
{
  // This is the Encoder's worker routine. It will physically read the hardware
  // and all most of the logic happens here. Recommended interval for this method is 1ms.
  clickEnc.service();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("init");
  pinMode(LED_BUILTIN, OUTPUT);
  display.clear();
  display.setSegments(TTD, 1, 0);
  display.setBrightness(7);

  pixel.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixel.setBrightness(7 * 8);
  pixel.show();

  // Setup and configure "full-blown" ClickEncoder
  clickEnc.setAccelerationEnabled(false);
  clickEnc.setDoubleClickEnabled(true);
  clickEnc.setLongPressRepeatEnabled(false);

  timer.initialize(TIMER_NOTIFY_US);
  timer.attachInterrupt(timerIsr);
  start = millis();
  randomSeed(analogRead(0));

  while (lcd.begin(COLUMS, ROWS, LCD_5x8DOTS) != 1) // colums, rows, characters size
  {
    Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
    delay(5000);
  }

  lcd.clear();
  Serial.println(COM);

  initGame();
  //  htcom = HTCOM(COM_PIN, 44);
  htcom = HTCOM();
  htcom.attach(COM_PIN, 44);
  htcom.set_SerialNumber(serialNumber);
}

void initGame()
{
  generateSerialNumber();

  generateIndicators();

  printStatusLine();

  resetStrikes();
}

long count = 0;

void loop()
{
  htcom.poll();
  delay(10);
  count++;
  int act = MAX_TIME - ((millis() - start) / 1000);
  showTime(act);

  if (count > 100)
  {
    strikes[0] = true;
  }
  if (count > 200)
  {
    strikes[1] = true;
  }
  if (count > 300)
  {
    strikes[2] = true;
  }
  showStrikes();
  printClickEncoderButtonState();
  printClickEncoderValue();
  printClickEncoderCount();
}

void generateSerialNumber()
{
  char a = random(26) + 'A';
  serialNumber[0] = a;
  a = random(26) + 'A';
  serialNumber[1] = a;

  a = random(10) + '0';
  serialNumber[2] = a;

  a = random(26) + 'A';
  serialNumber[3] = a;
  a = random(26) + 'A';
  serialNumber[4] = a;

  a = random(10) + '0';
  serialNumber[5] = a;
  Serial.print("Serialnumber is: ");

  strncpy(buffer, serialNumber, 6);
  buffer[6] = 0x00;

  Serial.println(buffer);
}

void generateIndicators()
{
  uint8_t indCount = random(1, 4);
#ifdef debug
  Serial.print("ind: ");
  Serial.print(indCount);
  Serial.println();
#endif

  for (uint8_t x = 0; x < sizeof(indicators); x++)
  {
    indicators[x] = 0;
    if (x < indCount)
    {
      indicators[x] = random(INDICATOR_COUNT) + 1;
    }
  }
#ifdef debug
  for (uint8_t x = 0; x < sizeof(indicators); x++)
  {
    uint8_t idx = indicators[x];
    strcpy_P(buffer, (char *)pgm_read_word(&(INDICATORNAMES[idx])));
    Serial.print(idx);
    Serial.print(" ");
    Serial.println(buffer);
  }
  Serial.println();
#endif
}

void printStatusLine()
{
  clearRow(3);
  lcd.print("S:");
  strncpy(buffer, serialNumber, 6);
  buffer[6] = 0x00;
  lcd.print(buffer);
  lcd.print(" ");

  for (uint8_t x = 0; x < sizeof(indicators); x++)
  {
    if (indicators[x] > 0)
    {
      uint8_t idx = indicators[x];
      strcpy_P(buffer, (char *)pgm_read_word(&(INDICATORNAMES[idx])));
      lcd.print(buffer);
      lcd.print(" ");
    }
  }
}

int saveTime = 0;
void showTime(int act)
{
  if (act != saveTime)
  {
    saveTime = act;
    lcd.setCursor(15, 0);
    bool neg = act < 0;
    int t = abs(act);
    byte sec = t % 60;
    byte min = (t - sec) / 60;
    if (neg)
    {
      lcd.print("-");
      display.setSegments(MND, 1, 0);
      lcd.print(min);
      display.showNumberDec(min, false, 1, 1);
    }
    else
    {
      lcd.print(min);
      display.showNumberDecEx(min, 0b01000000 & (sec % 2) << 6, false, 2, 0);
    }
    lcd.print(":");
    lcd.print(sec);
    display.showNumberDec(sec, true, 2, 2);
  }
}

void printClickEncoderButtonState()
{
  switch (clickEnc.getButton())
  {
  case Button::Clicked:
    clearRow(2);
    lcd.print("Button clicked");
    beep();
    break;
  case Button::DoubleClicked:
    clearRow(2);
    lcd.print("Button doubleClicked");
    dblBeep();
    break;
  case Button::Held:
    clearRow(2);
    lcd.print("Button Held");
    break;
  case Button::LongPressRepeat:
    clearRow(2);
    lcd.print("Button longPressRepeat");
    break;
  case Button::Released:
    clearRow(2);
    lcd.print("Button released");
    break;
  default:
    // no output for "Open" to not spam the terminal.
    break;
  }
}

void clearRow(uint8_t row)
{
  lcd.setCursor(0, row);
  lcd.print("                    ");
  lcd.setCursor(0, row);
}

void printClickEncoderValue()
{
  int16_t value = clickEnc.getIncrement();
  if (value != 0)
  {
    clearRow(1);
    lcd.print("Enc: ");
    lcd.print(value, PRINT_BASE);
  }
}

void printClickEncoderCount()
{
  static int16_t lastValue{0};
  int16_t value = clickEnc.getAccumulate();
  if (value != lastValue)
  {
    lcd.print(", c: ");
    lcd.print(value, PRINT_BASE);
  }
  lastValue = value;
}

void dblBeep()
{
  tone(BEEP_PIN, 440, 100);
  delay(200);
  tone(BEEP_PIN, 440, 100);
  delay(100);
  pinMode(BEEP_PIN, INPUT);
}

void beep()
{
  tone(BEEP_PIN, 440, 100);
  delay(100);
  pinMode(BEEP_PIN, INPUT);
}

void resetStrikes()
{
  for (uint8_t x = 0; x < sizeof(strikes); x++)
  {
    strikes[x] = false;
  }
}

void showStrikes()
{
  for (uint8_t x = 0; x < sizeof(strikes); x++)
  {
    pixel.setPixelColor(x, GREEN);
    if (strikes[x])
    {
      pixel.setPixelColor(x, RED);
      lcd.setCursor(10 + x, 0);
      lcd.print('*');
    }
  }
  pixel.show();
}

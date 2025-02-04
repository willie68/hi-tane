#include <Arduino.h>
#include <avdweb_Switch.h>

#define debug
#include <debug.h>
#include <game.h>

// RGB LED
#define LED_PIN 4
#define COM_PIN 11

#define LED_RED 2
#define BTN_RED 3
#define LED_BLUE 5
#define BTN_BLUE 6
#define LED_YELLOW 7
#define BTN_YELLOW 8
#define LED_GREEN 9
#define BTN_GREEN 10

Switch btRed = Switch(BTN_RED);
Switch btBlue = Switch(BTN_BLUE);
Switch btGreen = Switch(BTN_GREEN);
Switch btYellow = Switch(BTN_YELLOW);

enum Colors
{
  RED = 0,
  BLUE,
  GREEN,
  YELLOW
};

const byte color2pin[] = {LED_RED, LED_BLUE, LED_GREEN, LED_YELLOW};

// color validation schema matrixes
// snr has vocal, no error
Colors mx_hv_ne[4] = {BLUE, RED, YELLOW, GREEN};
// snr has vocal, one error
Colors mx_hv_oe[4] = {YELLOW, GREEN, BLUE, RED};
// snr has vocal, two error
Colors mx_hv_te[4] = {GREEN, RED, YELLOW, BLUE};
// snr has no vocal, no error
Colors mx_nv_ne[4] = {BLUE, YELLOW, GREEN, RED};
// snr has no vocal, one error
Colors mx_nv_oe[4] = {RED, BLUE, YELLOW, GREEN};
// snr has no vocal, two error
Colors mx_nv_te[4] = {YELLOW, GREEN, BLUE, RED};

// Game framework
Game game(ModuleTag::SIMON, LED_PIN, COM_PIN);

// --- forward functions
void initGame();
void LedOn(Colors color, bool on);
void blink(Colors color);
void btnPoll();
void ledPoll();
void showStep();
void calcValidationSchema();

const byte STEPS_SIMPLE = 6;
const byte STEPS_MEDIUM = 8;
const byte STEPS_HARD = 10;

byte stepCount;
Colors steps[STEPS_HARD];
Colors *validationSchema;

void setup()
{
  Serial.begin(115200);
  Serial.println("init simon");
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  pinMode(BTN_RED, INPUT_PULLUP);
  pinMode(BTN_BLUE, INPUT_PULLUP);
  pinMode(BTN_GREEN, INPUT_PULLUP);
  pinMode(BTN_YELLOW, INPUT_PULLUP);

  randomSeed(analogRead(0));
  initGame();
  game.arm();
}

// the actual step we're in
byte step;
byte sstep;
Colors actColor;
Colors expColor;

void loop()
{
  game.poll();
  btnPoll();
  ledPoll();

  if (step != sstep)
  {
    sstep = step;
    showStep();
  }

  game.showTime();
}

void initGame()
{
  game.init();
  switch (game.getGameDifficulty())
  {
  case Difficulty::MEDIUM:
    stepCount = STEPS_MEDIUM;
    break;
  case Difficulty::HARD:
    stepCount = STEPS_HARD;
    break;
  default:
    stepCount = STEPS_SIMPLE;
  }
  for (byte i = 0; i < STEPS_HARD; i++)
  {
    steps[i] = static_cast<Colors>(random(0, 4));
    dbgOut("Step ");
    dbgOut(i);
    dbgOut(":");
    dbgOutLn(steps[i]);
  }
  step = 0;
  sstep = 255;
}

void showStep()
{
  calcValidationSchema();
  actColor = steps[step];
  LedOn(actColor, true);

  expColor = validationSchema[actColor];

  Serial.print(actColor);
  Serial.print("->");
  Serial.println(expColor);
}

void calcValidationSchema()
{
  if (game.snrHasVocal())
  {
    switch (game.getStrikes())
    {
    case 0:
      dbgOutLn("mx_hv_ne");
      validationSchema = mx_hv_ne;
      break;
    case 1:
      dbgOutLn("mx_hv_oe");
      validationSchema = mx_hv_oe;
      break;
    case 2:
      dbgOutLn("mx_hv_te");
      validationSchema = mx_hv_te;
      break;
    }
  }
  else
  {
    switch (game.getStrikes())
    {
    case 0:
      dbgOutLn("mx_nv_ne");
      validationSchema = mx_nv_ne;
      break;
    case 1:
      dbgOutLn("mx_nv_oe");
      validationSchema = mx_nv_oe;
      break;
    case 2:
      dbgOutLn("mx_nv_te");
      validationSchema = mx_nv_te;
      break;
    }
  }
}

long ledTimers[4];
void blink(Colors color)
{
  LedOn(color, true);
  ledTimers[color] = millis() + 500;
}

void ledPoll()
{
  long act = millis();
  for (byte i = 0; i < 4; i++)
    if (ledTimers[i] > 0)
      if (ledTimers[i] < act)
      {
        ledTimers[i] = 0;
        LedOn(static_cast<Colors>(i), false);
      }
}

void LedOn(Colors color, bool on)
{
  byte pin = color2pin[color];
  digitalWrite(pin, on);
}

void btnPoll()
{
  btBlue.poll();
  btRed.poll();
  btYellow.poll();
  btGreen.poll();
}
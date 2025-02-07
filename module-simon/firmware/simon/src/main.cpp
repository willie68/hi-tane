#include <Arduino.h>
#include <avdweb_Switch.h>

#define debug
#include <debug.h>
#include <game.h>
#include <button.h>

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

Button btRed = Button(LED_RED, BTN_RED, RED);
Button btBlue = Button(LED_BLUE, BTN_BLUE, BLUE);
Button btGreen = Button(LED_GREEN, BTN_GREEN, GREEN);
Button btYellow = Button(LED_YELLOW, BTN_YELLOW, YELLOW);

const Button color2btn[] = {btRed, btBlue, btGreen, btYellow};

// color validation schema matrixes
// no validation, simple simon says
Colors mx_si_no[4] = {RED, BLUE, GREEN, YELLOW};
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
void poll();
void showStep();
void calcValidationSchema();
bool notBtnColorCLicked(Colors color);
bool btnColorClicked(Colors color);
void nextStep();

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
  poll();

  if (step != sstep)
  {
    sstep = step;
    showStep();
  }
  if (notBtnColorCLicked(expColor))
  {
    Serial.println("strike");
  }
  if (btnColorClicked(expColor))
  {
    nextStep();
  }

  game.showTime();
}

void nextStep()
{
  if (step < stepCount)
  {
    step++;
    calcValidationSchema();
    return;
  }
  else
  {
    game.setSolved();
  }
}

bool notBtnColorCLicked(Colors color)
{
  switch (color)
  {
  case RED:
    return btGreen.singleClick() || btBlue.singleClick() || btYellow.singleClick();
  case GREEN:
    return btRed.singleClick() || btBlue.singleClick() || btYellow.singleClick();
  case BLUE:
    return btGreen.singleClick() || btRed.singleClick() || btYellow.singleClick();
  case YELLOW:
    return btGreen.singleClick() || btRed.singleClick() || btBlue.singleClick();
  }
  return false;
}

bool btnColorClicked(Colors color)
{
  switch (color)
  {
  case RED:
    return btRed.singleClick();
  case GREEN:
    return btGreen.singleClick();
  case BLUE:
    return btBlue.singleClick();
  case YELLOW:
    return btYellow.singleClick();
  }
  return false;
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
#ifdef debug
  dbgOutLn("mx_si_no");
  validationSchema = mx_si_no;
#else
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
#endif
}

void LedOn(Colors color, bool on)
{
  Button btn = color2btn[color];
  btn.LED(on);
}

void btnPoll()
{
  game.poll();

  btBlue.poll();
  btRed.poll();
  btYellow.poll();
  btGreen.poll();
}
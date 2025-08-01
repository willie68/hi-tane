#include <Arduino.h>
#include <avdweb_Switch.h>

#define debug
#include <debug.h>
#include <game.h>
#include <button.h>

#define simpleval

// RGB LED
const uint8_t LED_PIN = 4;
const uint8_t COM_PIN = 2;

const uint8_t LED_RED = 3;
const uint8_t BTN_RED = 14;
const uint8_t LED_BLUE = 5;
const uint8_t BTN_BLUE = 7;
const uint8_t LED_YELLOW = 6;
const uint8_t BTN_YELLOW = 8;
const uint8_t LED_GREEN = 9;
const uint8_t BTN_GREEN = 15;

Button btRed = Button(LED_RED, BTN_RED, RED);
Button btBlue = Button(LED_BLUE, BTN_BLUE, BLUE);
Button btGreen = Button(LED_GREEN, BTN_GREEN, GREEN);
Button btYellow = Button(LED_YELLOW, BTN_YELLOW, YELLOW);

// color validation schema matrixes
// no validation, simple simon says
Color mx_si_no[4] = {RED, BLUE, GREEN, YELLOW};
// snr has vocal, no error
Color mx_hv_ne[4] = {BLUE, RED, YELLOW, GREEN};
// snr has vocal, one error
Color mx_hv_oe[4] = {YELLOW, GREEN, BLUE, RED};
// snr has vocal, two error
Color mx_hv_te[4] = {GREEN, RED, YELLOW, BLUE};
// snr has no vocal, no error
Color mx_nv_ne[4] = {BLUE, YELLOW, GREEN, RED};
// snr has no vocal, one error
Color mx_nv_oe[4] = {RED, BLUE, YELLOW, GREEN};
// snr has no vocal, two error
Color mx_nv_te[4] = {YELLOW, GREEN, BLUE, RED};

// Game framework
Game game(ModuleTag::SIMON, LED_PIN);

enum TriState
{
  TRUE = 1,
  FALSE,
  NONE
};

// --- forward functions
void initGame();
void LedOn(Color color, bool on);
void poll();
void showStep();
void calcValidationSchema();
TriState btnColorClicked(Color color);
void nextStep();
void allLEDOff();
void allLEDOn();
void printMX(Color mx[4]);
void showSolveEffekt();

const byte STEPS_SIMPLE = 6;
const byte STEPS_MEDIUM = 8;
const byte STEPS_HARD = 10;

byte stepCount;
Color steps[STEPS_HARD];
Color *validationSchema;

void setup()
{
  Serial.begin(115200);
  Serial.println("init simon");

  randomSeed(analogRead(0));

  game.init();

  initGame();
}

// the actual step we're in
byte step;
byte sstep;

void initGame()
{
  game.setState(ModuleState::INIT);
  dbgOutLn("init game");
  Difficulty dif = game.getGameDifficulty();
  switch (dif)
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
    if (i >= stepCount)
    {
      steps[i] = Color::NN;
      continue;
    }
    byte step = random(0, 4);
    if (i > 0)
    {
      while (steps[i - 1] == step)
      {
        step = random(0, 4);
      }
    }
    steps[i] = static_cast<Color>(step);
    dbgOut(F("Step "));
    dbgOut(i);
    dbgOut(F(":"));
    dbgOutLn(steps[i]);
  }
  step = 0;
  sstep = 255;
  game.arm();
}

Color actColor;
Color expColor;
TriState state;

// some nice trisel for solved the game
byte trStep = 0;
unsigned long trTime = 0;

void loop()
{
  poll();
  if (game.isState(ModuleState::DISARMED))
    showSolveEffekt();

  if (step != sstep)
  {
    sstep = step;
    showStep();
  }
  state = btnColorClicked(expColor);
  switch (state)
  {
  case TRUE:
    nextStep();
    dbgOut("Step ");
    dbgOut(step);
    dbgOut("(");
    dbgOut(sstep);
    dbgOutLn(")");

    break;
  case FALSE:
    game.setStrike();
    step = 0;
    break;
  default:
    break;
    // do nothing is ok here
  }
  // game.showTime(true);
}

void showSolveEffekt()
{
  if (trTime < millis())
  {
    btRed.LEDOff();
    btBlue.LEDOff();
    btGreen.LEDOff();
    btYellow.LEDOff();
    trTime = millis() + 250;
    trStep++;
  }
  switch (trStep % 4)
  {
  case 0:
    btRed.LED(false);
    break;
  case 1:
    btBlue.LED(false);
    break;
  case 2:
    btYellow.LED(false);
    break;
  case 3:
    btGreen.LED(false);
    break;
  }
}

void nextStep()
{
  if (step < (stepCount - 1))
  {
    step++;
    calcValidationSchema();
    return;
  }
  else
  {
    allLEDOn();
    delay(1000);
    allLEDOff();
    game.setSolved();
  }
}

TriState btnColorClicked(Color color)
{
  TriState state = NONE;
  switch (color)
  {
  case RED:
    if (btRed.clicked())
      state = TRUE;
    break;
  case GREEN:
    if (btGreen.clicked())
      state = TRUE;
    break;
  case BLUE:
    if (btBlue.clicked())
      state = TRUE;
    break;
  case YELLOW:
    if (btYellow.clicked())
      state = TRUE;
    break;
  }
  if (state == TRUE)
  {
    return state;
  }
  switch (color)
  {
  case RED:
    if (btGreen.clicked() || btBlue.clicked() || btYellow.clicked())
      state = FALSE;
    break;
  case GREEN:
    if (btRed.clicked() || btBlue.clicked() || btYellow.clicked())
      state = FALSE;
    break;
  case BLUE:
    if (btGreen.clicked() || btRed.clicked() || btYellow.clicked())
      state = FALSE;
    break;
  case YELLOW:
    if (btGreen.clicked() || btRed.clicked() || btBlue.clicked())
      state = FALSE;
    break;
  case NN:
    break;
  }
  return state;
}

void printMX(Color mx[4])
{
  Color c;
  for (byte i = 0; i < 4; i++)
  {
    c = mx[i];
    dbgOut(c);
    dbgOut(",");
  }
  dbgOutLn();
}

void showStep()
{
  dbgOut("show step: ");
  dbgOutLn(step);
  allLEDOff();
  delay(100);
  calcValidationSchema();
  actColor = steps[step];
  LedOn(actColor, true);

  expColor = validationSchema[static_cast<byte>(actColor)];

  dbgOut(actColor);
  dbgOut(F("->"));
  dbgOutLn(expColor);
}

void calcValidationSchema()
{
  dbgOut("strikes: ");
  dbgOutLn(game.getStrikes());
  if (game.snrHasVocal())
  {
    dbgOutLn("snr has vocal");
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
    dbgOutLn("snr has no vocal");
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
#ifdef simpleval
  dbgOutLn("mx_si_no");
  validationSchema = mx_si_no;
#endif
  dbgOut("validation: ");
  printMX(validationSchema);
}

void LedOn(Color color, bool on)
{
  switch (color)
  {
  case Color::RED:
    btRed.LED(on);
    break;
  case Color::BLUE:
    btBlue.LED(on);
    break;
  case Color::GREEN:
    btGreen.LED(on);
    break;
  case Color::YELLOW:
    btYellow.LED(on);
    break;
  }
}

void poll()
{
  game.poll();
  if (game.isNewGameSettings())
  {
    initGame();
  }
  if (game.hasNewStrikes())
  {
    showStep();
  }

  btBlue.poll();
  btRed.poll();
  btYellow.poll();
  btGreen.poll();
}

void allLEDOff()
{
  btBlue.LED(0);
  btRed.LED(0);
  btYellow.LED(0);
  btGreen.LED(0);
}

void allLEDOn()
{
  btBlue.LED(1);
  btRed.LED(1);
  btYellow.LED(1);
  btGreen.LED(1);
}

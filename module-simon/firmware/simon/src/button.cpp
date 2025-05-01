#include <button.h>

const byte BTN_LED_OFF = 0;
const byte BTN_LED_INACTIVE = 20;
const byte BTN_LED_ACTIVE = 255;

Button::Button(byte ledPin, byte switchPin, Color color)
{
    btn = new Switch(switchPin);
    this->ledPin = ledPin;
    pinMode(ledPin, OUTPUT);
    this->color = color;
};

byte Button::pin()
{
    return ledPin;
}

void Button::poll()
{
    btn->poll();
    if (off)
    {
        setPWM(BTN_LED_OFF);
        return;
    }
    if (this->active)
    {
        if ((millis() % 1000L) < 500L)
        {
            setPWM(BTN_LED_ACTIVE);
        }
        else
        {
            setPWM(BTN_LED_INACTIVE);
        }
    }
    else
    {
        setPWM(BTN_LED_INACTIVE);
    }
}

void Button::setPWM(byte value)
{
    if (lastValue != value)
    {
        analogWrite(ledPin, value);
        lastValue = value;
    }
}

bool Button::clicked()
{
    return btn->singleClick();
}

void Button::LED(bool on)
{
    active = on;
    off = false;
}

void Button::LEDOff()
{
    active = false;
    off = true;
    setPWM(BTN_LED_OFF);
}
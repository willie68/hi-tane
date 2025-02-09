#include <button.h>
#define debug
#include <debug.h>

Button::Button(byte ledPin, byte switchPin, Color color) 
{
    btn = new Switch(switchPin);
    this->ledPin = ledPin;
    pinMode(ledPin, OUTPUT);
    this->color = color;
};

void Button::poll() {
    btn->poll();
}

bool Button::clicked() {
    return btn->singleClick();
}

void Button::LED(bool on) {
    if (on) {
        analogWrite(ledPin, 255);
    } else {
        analogWrite(ledPin, 20);
    }
}

void Button::LEDOff() {
    analogWrite(ledPin, 0);
}
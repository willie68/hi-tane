#include <button.h>
#define debug
#include <debug.h>

Button::Button(byte ledPin, byte switchPin, Colors color) 
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
    digitalWrite(ledPin, on);
}
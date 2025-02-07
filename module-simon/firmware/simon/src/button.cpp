#include <button.h>

Button::Button(byte ledPin, byte switchPin, Colors color) 
{
    btn = &Switch(switchPin);
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
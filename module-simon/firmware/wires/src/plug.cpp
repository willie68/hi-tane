// Implementation of a single plug in the patch panel
#include <Arduino.h>
#include <plug.h>

Plug::Plug(byte pin)
{
  this->pin = pin;

  installed = actual();
}

bool Plug::hasWire()
{
  return installed.color != WIRECOLORS::NONE;
};

bool Plug::actualHasWire()
{
  return actual().color != WIRECOLORS::NONE;
};

Wire Plug::initial()
{
  return installed;
};

Wire Plug::actual()
{
  value = analogRead(pin);
  return getWireFromADV(value);
};

int Plug::getValue()
{
  return value;
}
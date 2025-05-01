// Implementation of a single plug in the patch panel
#include <Arduino.h>
#include <plug.h>

Plug::Plug(byte pin)
{
  this->m_pin = pin;

  m_installed = actual();
}

bool Plug::hasWire()
{
  return m_installed.color != WIRECOLORS::NONE;
};

bool Plug::actualHasWire()
{
  return actual().color != WIRECOLORS::NONE;
};

Wire Plug::initial()
{
  return m_installed;
};

Wire Plug::actual()
{
  m_value = analogRead(m_pin);
  return getWireFromADV(m_value);
};

int Plug::getValue()
{
  return m_value;
}
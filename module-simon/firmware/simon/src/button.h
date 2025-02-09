#ifndef BUTTON_H
#define BUTTON_H

#include <avdweb_Switch.h>

enum Color
{
  RED = 0,
  BLUE,
  GREEN,
  YELLOW
};

class Button
{
    public:
      Button(byte ledPin, byte switchPin, Color color);
      void poll();
      bool  clicked();
      void LED(bool on);
      void LEDOff();
    private:
        Switch *btn;
        byte ledPin;
        Color color;
};

#endif
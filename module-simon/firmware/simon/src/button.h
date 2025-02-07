#ifndef BUTTON_H
#define BUTTON_H

#include <avdweb_Switch.h>

enum Colors
{
  RED = 0,
  BLUE,
  GREEN,
  YELLOW
};

class Button
{
    public:
      Button(byte ledPin, byte switchPin, Colors color);
      void poll();
      bool  clicked();
      void LED(bool on);
    private:
        Switch *btn;
        byte ledPin;
        Colors color;
};

#endif
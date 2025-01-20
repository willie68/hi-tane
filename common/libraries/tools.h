#include <Arduino.h>
#ifndef TOOLS_H
#define TOOLS_H
void hex(byte x) {
   if (x < 16) {Serial.print("0");}
   Serial.print(x, HEX);
}
#endif
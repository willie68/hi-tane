# Controller

Hier befindet sich die Firmware des Controllers.

## Umschaltung zwischen den Versionen 1 und 2

Die Version 1 ist eine Version für ein 20x4 I²C LC-Display und TM1637 7-Segment Display. 
Die Version 2 ersetzt das LC-Display durch ein 128x64 1,3" OLED mit dem Controller SH1106. 

Für beide Versionen ist auch eine entsprechende Wokwi Simulator-Datei enthalten.

Zum Umschalten reicht es in der display.h Zeile 9ff das entsprechende #define zu entkommentieren und das nicht benutzte wieder zu kommentieren. 

z.B. für das OLED Display

```cpp
// only one of this should be active
//#define NN
//#define LCD
#define OLED
```

oder das LCD

```cpp
// only one of this should be active
//#define NN
#define LCD
//#define OLED
```

Will man auch die wokwi Simulation entsprechend umschalten, öffne bitte ein terminal auf dem firmware-root und benutze die beiden Scripte `.\scripts\tooled.cmd` bzw `.\scripts\tolcd.cmd`.
Diese kopieren die entsprechende diagram.json. Ein Reload des Simulators ist notwendig.
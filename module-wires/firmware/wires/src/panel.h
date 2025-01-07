// Definition of the patch panel component
#ifndef PANEL_H
#define PANEL_H
#include <plug.h>

class Panel
{
public:
    Panel();
    bool init();
    
    bool isUnarmed();
private:
    Plug *plugs;
    byte thePlug;

    byte countColor(WIRECOLORS color);
    bool isLastColor(WIRECOLORS color);
    byte getPlug(byte position);
    byte getLastPlug();
    byte getLastPlugOfColor(WIRECOLORS color);
};
#endif
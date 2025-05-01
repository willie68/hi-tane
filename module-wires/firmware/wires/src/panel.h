// Definition of the patch panel component
#ifndef PANEL_H
#define PANEL_H
#include <plug.h>

class Panel
{
public:
    Panel();
    bool init(bool is_sn_last_digit_odd);

    bool changed();
    bool isDisarmed();
    bool isStriken();

    void printPlugs();

private:
    bool m_sn_last_digit_odd;
    Plug *m_plugs;
    byte m_defusePlug;

    byte get3WireDefusePlug();
    byte get4WireDefusePlug();
    byte get5WireDefusePlug();
    byte get6WireDefusePlug();

    byte countColor(WIRECOLORS color);
    bool isLastColor(WIRECOLORS color);
    byte getPlug(byte position);
    byte getLastPlug();
    byte getLastPlugOfColor(WIRECOLORS color);
};
#endif
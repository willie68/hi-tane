#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#define Version "V0.1 " // always add an space at the end

class CtrlDisplay
{
    virtual void init(byte brightness) = 0;

    virtual void setBrightness(byte brightness) = 0;
    virtual void setDifficulty(byte difficulty) = 0;

    // time display
    virtual void showTime(long act) = 0;
    virtual void clearTime() = 0;

    // info display
    virtual void clear() = 0;
    virtual void printHeader(bool withVersion) = 0;
    virtual void printWelcome() = 0;
    virtual void showError(byte error) = 0;
};

class LCDisplay : public CtrlDisplay
{
public:
    LCDisplay();
    void init(byte brightness) override;

    void setBrightness(byte brightness) override;
    void setDifficulty(byte difficulty) override;

    // time display
    void showTime(long act) override;
    void clearTime() override;

    // info display
    void clear() override;
    void printHeader(bool withVersion) override;
    void printWelcome() override;
    void showError(byte error) override;

private:
    byte m_brightness;
    byte m_difficulty;
};

#endif
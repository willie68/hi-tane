#include <morse.h>
#include "Arduino.h"
#define debug
#include <debug.h>

Morse::Morse(byte ls, byte led, long dit)
{
    _ls = ls;
    _led = led;
    _dit = dit;
    _bls = ls > 0;
    _bled = led > 0;
    _mend = true;
    if (_bls)
    {
        noTone(_ls);
        //    dbgOut("LS on ");
        //    dbgOut(_ls);
    }
    if (_bled)
    {
        digitalWrite(_led, 0);
        //    dbgOut("LEB on ");
        //    dbgOut(_led);
    }
    calculateLengths();
}

void Morse::setDitLength(word dl)
{
    _dit = dl;
    calculateLengths();
}

void Morse::calculateLengths()
{
    _dah  = 3 * _dit;
    _symPause = _dit;
    _charPause = 3 * _dit;
    _wordPause = 7 * _dit;
}

void Morse::sendMessage(const char *msg)
{
    dbgOut("morsing:");
    dbgOutLn(msg);
    reset();
    _msg = msg;
    _pos = 0;
    _cend = true;
    _mend = false;
    _mit = false;
    _pse = true;
    _t = millis() + _wordPause;

    /*   if (x > 0) {
          delay(charPause);
          Serial.print(" ");
        }
    */
}

void Morse::reset()
{
    if (_bls)
        noTone(_ls);
    if (_bled)
        digitalWrite(_led, 0);
    _pos = 0;
    _cend = true;
    _mend = false;
    _mit = false;
    _pse = false;
}

bool Morse::finished()
{
    return _mend;
}

void Morse::poll()
{
    if (_mend)
    {
        //    dbgOutLn("msg end");
        return;
    }
    if (_cend)
    {
        dbgOut("cend, ");
        // get next char
        if (_msg[_pos] == 0)
        {
            // end of message
            _mend = true;
            return;
        }
        // convert it to morse entry
        _ch = _msg[_pos];
        dbgOut(", ");
        dbgOut(_ch);
        if ((_ch >= 'a') && (_ch <= 'z'))
        {
            _ch = alphabet[_ch - 'a'];
        }
        else if ((_ch >= 'A') && (_ch <= 'Z'))
        {
            _ch = alphabet[_ch - 'A'];
        }
        else if ((_ch >= '0') && (_ch <= '9'))
        {
            _ch = alphabet[_ch - '0' + 26];
        }
        dbgOut("->");
        dbgOut2(_ch, HEX);

        _cend = false;
        _mit = false;
        _pse = false;
        _pos++;
    }

    long act = millis();
    if ((!_mit) && (!_pse))
    {
        // wenn gerade kein mit gesendet wird und keine pause ist
        // begin des mit.
        // Startzeit merken. mit einschalten. StopZeit merken
        _mcnt = (_ch & 0xF0) >> 5;
        _mdta = _ch & 0x1F;
        _pse = true;
        //    dbgOut(": ");
    }

    if (_t < act)
    {
        // schauen, ob die Zeit abgelaufen ist.
        if (_mcnt == 0)
        {
            // nächstes zeichen
            if (_bls)
                noTone(_ls);
            if (_bled)
                digitalWrite(_led, 0);
            _t = act + _charPause;
            _cend = true;
            _mit = false;
            _pse = false;
        }
        if (_mit)
        {
            // wenn gerade mit gesendet wird
            if (_bls)
                noTone(_ls);
            if (_bled)
                digitalWrite(_led, 0);
            _mit = false;
            _pse = true;
            _t = act + _dit;
        }
        else if (_pse)
        {
            // wenn gerade pause ist und die Pausenzeit abgelaufen ist.
            if (_bls)
                tone(_ls, mFreq);
            if (_bled)
                digitalWrite(_led, 1);
            _mit = true;
            _pse = false;
            // Wenn Pause abgelaufen, nächsten mit bestimmen
            if (_mdta & (1 << (_mcnt - 1)))
            {
                _t = act + _dah;
                //        dbgOut("-");
            }
            else
            {
                _t = act + _dit;
                //        dbgOut(".");
            }
            _mcnt--;
        }
    }
}

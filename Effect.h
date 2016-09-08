#ifndef Effect_h
#define Effect_h

#include "application.h"

class Effect
{
  public:
    Effect();
    Effect(int position, int length, int r, int g, int b, int style);

    bool _active;
    int  _position;
    int  _length;
    int  _r;
    int  _g;
    int  _b;
    int  _style;

    void activate();
    void deactivate();
};

#endif

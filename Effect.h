#ifndef Effect_h
#define Effect_h

#include "application.h"

class Effect
{
  public:
    Effect();

    void update(int start, int end, int r, int g, int b, int style);

    bool _active;
    long _startTime;

    int  _start;
    int  _end;
    int  _r;
    int  _g;
    int  _b;
    int  _style;

    void activate();
    void deactivate();
};

#endif

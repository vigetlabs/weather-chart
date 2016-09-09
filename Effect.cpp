#include "Effect.h"

Effect::Effect() {
  _active = false;
}

void Effect::update(int start, int end, int r, int g, int b, int style) {
  _active    = true;
  _startTime = millis();

  _start = start;
  _end   = end;
  _r     = r;
  _g     = g;
  _b     = b;
  _style = style;
}

void Effect::activate() {
  _active = true;
}

void Effect::deactivate() {
  _active = false;
}

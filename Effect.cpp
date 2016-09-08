#include "Effect.h"

Effect::Effect() {
  _active = false;
}

void Effect::update(int position, int length, int r, int g, int b, int style) {
  _active    = true;
  _startTime = millis();

  _position = position;
  _length   = length;
  _r        = r;
  _g        = g;
  _b        = b;
  _style    = style;
}

void Effect::activate() {
  _active = true;
}

void Effect::deactivate() {
  _active = false;
}

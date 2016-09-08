#include "LedStrip.h"

Adafruit_NeoPixel _strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, WS2812B);

LedStrip::LedStrip(int stepCount) {
  _stepCount = stepCount;
}

void LedStrip::initialize() {
  _strip.begin();
  _strip.setBrightness(255);
  _strip.show();
}

int LedStrip::addEffect(String input) {
  int checksum  = 0;
  int index     = 0;
  int lastIndex = 0;
  int value;
  int info[6];

  for (int i = 0; i < 6; i++) {
    index = input.indexOf(",", lastIndex);

    if (index == -1) {
      if (i != 5) {
        return -1;
      }
      index = input.length();
    }

    value     = atoi(input.substring(lastIndex, index));
    info[i]   = value;
    checksum += value;

    lastIndex = index + 1;
  }

  Effect newEffect       = Effect(info[0], info[1], info[2], info[3], info[4], info[5]);
  _effects[_effectCount] = newEffect;
  _effectCount           = (_effectCount + 1) % EFFECT_COUNT;

  return checksum;
}

void LedStrip::updatePositions(int stepperState[]) {
  float tracker = 0.5;

  for (int i = 0; i < 5; i++) {
    int x = map(stepperState[i],   0, _stepCount, 0, 100);
    int y = map(stepperState[i+1], 0, _stepCount, 0, 100);

    float gap = pow(144 + (0.0441 * pow(x - y, 2)), 0.5);
    _positions[i] = tracker;

    tracker += gap;
  }

  _positions[5] = tracker;
}

void LedStrip::updateState() {
  for (int i = 0; i < LED_COUNT; i++) {
    if (i < _positions[5]) {
      _targetState[i] = _strip.Color(0, 100, 100);
    } else {
      _targetState[i] = _strip.Color(0, 0, 0);
    }
  }

  for (int i = 0; i < 6; i++) {
    _targetState[_positions[i]] = _strip.Color(255, 0, 0);
  }
}

void LedStrip::show() {
  if (_shouldChangeLeds()) {
    for (int i = 0; i < LED_COUNT; i++) {
      _strip.setPixelColor(i, _targetState[i]);
      _currentState[i] = _targetState[i];
    }

    _strip.show();
  }
}

bool LedStrip::_shouldChangeLeds() {
  for (int i = 0; i < LED_COUNT; i++) {
    if (_currentState[i] != _targetState[i]) {
      return true;
    }
  }

  return false;
}

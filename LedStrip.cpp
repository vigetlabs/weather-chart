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

  _effects[_effectCount].update(info[0], info[1], info[2], info[3], info[4], info[5]);
  _effectCount           = (_effectCount + 1) % EFFECT_COUNT;

  return checksum;
}

void LedStrip::clearEffects() {
  for (int i = 0; i < EFFECT_COUNT; i++) {
    _effects[i].deactivate();
    _effectCount = 0;
  }
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
  for (size_t i = 0; i < LED_COUNT; i++) {
    _targetState[i] = 0;
  }

  for (int i = 0; i < EFFECT_COUNT; i++) {
    if (_effects[i]._active) {
      _layerOnEffect(i);
    }
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

void LedStrip::_layerOnEffect(int i) {
  Effect effect = _effects[i];
  long timeElapsed;
  int  index;

  switch (effect._style) {
    case 0:
      // Solid
      for (int j = _ledIndexFor(effect._start); j < _ledIndexFor(effect._end); j++) {
        _targetState[j] = _strip.Color(effect._r, effect._g, effect._b);
      }
      break;

    case 1:
      // Ants marching
      timeElapsed = millis() - effect._startTime;
      // 200: how quick the ants march (lower number = faster marching), 3: ant spacing
      index = (timeElapsed / 200) % 3;

      for (int j = _ledIndexFor(effect._start); j < _ledIndexFor(effect._end); j++) {
        if ((j % 3) == index) {
          _targetState[j] = _strip.Color(effect._r, effect._g, effect._b);
        }
      }
      break;

    case 2:
      // Breathing
      timeElapsed = millis() - effect._startTime;
      index       = (timeElapsed / 75) % 40; // 40 steps & 2 seconds per revolution
      float multiplier;
      if (index < 20) {
        multiplier = map(index, 0, 20, 5, 100) / 100.0;
      } else {
        multiplier = map(index, 20, 40, 100, 5) / 100.0;
      }

      for (int j = _ledIndexFor(effect._start); j < _ledIndexFor(effect._end); j++) {
        _targetState[j] = _strip.Color(effect._r * multiplier, effect._g * multiplier, effect._b * multiplier);
      }
      break;

    default:
      break;
  }
}

int LedStrip::_ledIndexFor(int position) {
  position = max(0, position);
  position = min(position, 100);

  for (int i = 0; i < 5; i++) {
    if (position <= ((i + 1) * 20)) {
      return map(position, i * 20, (i + 1) * 20, _positions[i], _positions[i + 1]);
    }
  }

  return _positions[5];
}

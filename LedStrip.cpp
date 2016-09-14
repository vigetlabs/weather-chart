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

void LedStrip::temperature(String input) {
  int checksum  = 0;
  int index     = 0;
  int lastIndex = 0;
  int value;

  for (int i = 0; i < 6; i++) {
    index = input.indexOf(",", lastIndex);

    if (index == -1) {
      if (i != 5) {
        return;
      }
      index = input.length();
    }

    value             = atoi(input.substring(lastIndex, index));
    _temperatures[i]  = value;
    checksum         += value;

    lastIndex = index + 1;
  }
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

    if (i < _positions[5]) {
      _calculateTemperature(i);
    }
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

void LedStrip::_calculateTemperature(int led) {
  int segment = _segmentFor(led);

  int ledTemperature = map(led,
    _positions[segment],
    _positions[segment + 1],
    _temperatures[segment],
    _temperatures[segment + 1]
  );

  _targetState[led] = _tempToColor(ledTemperature);
}

int LedStrip::_segmentFor(int led) {
  for (int i = 1; i < 6; i++) {
    if (led <= _positions[i]) {
      return i - 1;
    }
  }

  return 5;
}

uint32_t LedStrip::_tempToColor(int temperature) {
  int r, g, b;

  // -40 : violet
  // 50  : purple
  // 60  : blue
  // 70  : green
  // 75  : yellow
  // 80+ : red

  if (temperature <= 40) {
    r = 255;
    g = 0;
    b = 150;
  } else if (temperature <= 50) {
    r = 255;
    g = 0;
    b = map(temperature, 40, 50, 150, 255);
  } else if (temperature <= 60) {
    r = map(temperature, 40, 60, 255, 0);
    g = 0;
    b = 255;
  } else if (temperature <= 70) {
    r = 0;
    g = map(temperature, 50, 70, 0, 255);
    b = map(temperature, 50, 70, 255, 0);
  } else if (temperature <= 75) {
    r = map(temperature, 70, 75, 0, 255);
    g = 255;
    b = 0;
  } else {
    r = 255;
    g = max(0, map(temperature, 75, 85, 255, 0));
    b = 0;
  }

  return _strip.Color(r, g, b);
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

#ifndef LedStrip_h
#define LedStrip_h

#include "application.h"
#include "neopixel/neopixel.h"
#include "math.h"
#include "Effect.h"

#define LED_PIN   D0
#define LED_COUNT 149

#define EFFECT_COUNT 5

class LedStrip
{
  public:
    LedStrip(int stepCount);
    void initialize();
    int  addEffect(String input);
    void updatePositions(int stepperState[]);
    void updateState();
    void show();

  private:
    int _stepCount    = 0;
    int _positions[6] = {0, 0, 0, 0, 0, 0};

    Effect _effects[EFFECT_COUNT];
    int _effectCount = 0;

    uint32_t _currentState[LED_COUNT];
    uint32_t _targetState[LED_COUNT];

    bool _shouldChangeLeds();
};

#endif

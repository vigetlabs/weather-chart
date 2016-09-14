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
    void temperature(String input);
    void clearEffects();
    void updatePositions(int stepperState[]);
    void updateState();
    void show();

  private:
    int _stepCount       = 0;
    int _positions[6]    = {0, 0, 0, 0, 0, 0};
    int _temperatures[6] = {87, 80, 71, 64, 56, 45};

    Effect _effects[EFFECT_COUNT];
    int _effectCount = 0;

    uint32_t _currentState[LED_COUNT];
    uint32_t _targetState[LED_COUNT];

    bool _shouldChangeLeds();
    void _calculateTemperature(int i);
    uint32_t _tempToColor(int temperature);
    void _layerOnEffect(int i);
    int  _ledIndexFor(int position);
    int  _segmentFor(int led);
};

#endif

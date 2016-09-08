#ifndef LedStrip_h
#define LedStrip_h

#include "application.h"
#include "neopixel/neopixel.h"
#include "math.h"

#define LED_PIN   D0
#define LED_COUNT 149

class LedStrip
{
  public:
    LedStrip(int stepCount);
    void initialize();
    void updatePositions(int stepperState[]);
    void updateState();
    void show();

  private:
    int _stepCount    = 0;
    int _positions[6] = {0, 0, 0, 0, 0, 0};

    uint32_t _currentState[LED_COUNT];
    uint32_t _targetState[LED_COUNT];

    bool _shouldChangeLeds();
};

#endif

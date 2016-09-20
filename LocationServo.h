#ifndef LocationServo_h
#define LocationServo_h

#include "application.h"
#include "math.h"

#define SERVO_PIN D1

#define LEFT_POSITION 16

#define MOVE_MULTIPLIER 40 // the higher this is, the slower the thing moves
#define POSITION_GAP 40

class LocationServo
{
  public:
    LocationServo();
    void setLocation(String location);
    void updateState(long currentTime);
    void show();

  private:
    int  _oldPosition     = LEFT_POSITION;
    int  _currentPosition = LEFT_POSITION;
    int  _targetPosition  = LEFT_POSITION;
    long _currentTime     = 0;
    long _startTime       = 0;
    long _timePassed      = 0;
    long _moveTime        = 0;

    void _determinePosition();
    bool _shouldMove();
};

#endif

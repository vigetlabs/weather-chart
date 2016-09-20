#include "LocationServo.h"

Servo _locationServo;

LocationServo::LocationServo() {
  _locationServo.attach(SERVO_PIN);
  _locationServo.write(LEFT_POSITION);
}

void LocationServo::setLocation(String location) {
  int newPosition;

  if (location.length() == 0) {
    return;
  } else if (location == "1") {
    newPosition = 16;
  } else if (location == "2") {
    newPosition = 60;
  } else if (location == "3") {
    newPosition = 113;
  } else if (location == "4") {
    newPosition = 161;
  }

  _startTime = _currentTime;
  _moveTime  = abs(newPosition - _currentPosition) * MOVE_MULTIPLIER;

  _oldPosition    = _currentPosition;
  _targetPosition = newPosition;
}

void LocationServo::updateState(long currentTime) {
  _currentTime = currentTime;

  if (_shouldMove()) {
    _timePassed = _currentTime - _startTime;
    _determinePosition();
  }
}

void LocationServo::show() {
  _locationServo.write(_currentPosition);
}

// private

void LocationServo::_determinePosition() {
  float movePercentage   = (_timePassed * 1.0) / _moveTime;
  int positionDifference = _targetPosition - _oldPosition;

  _currentPosition = _oldPosition + (movePercentage * positionDifference);
}

bool LocationServo::_shouldMove() {
  return _currentPosition != _targetPosition;
}

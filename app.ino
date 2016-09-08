#include "LedStrip.h"

#define STEP_COUNT (4450 * 4)

LedStrip lights = LedStrip(STEP_COUNT);

int  currentLastLed = 0;
bool shouldShowLeds = false;

int currentLedState[6] = {0, 0, 0, 0, 0, 0};
int targetLedState[6]  = {0, 0, 0, 0, 0, 0};

int DIR_SER   = D2;
int DIR_RCLK  = D3;
int DIR_SRCLK = D4;

int STEP_SER   = D5;
int STEP_RCLK  = D6;
int STEP_SRCLK = D7;

boolean dirRegister[8];
boolean stepRegister[8];

bool calibrated[6] = {false, false, false, false, false, false};

int currentState[6] = {0, 0, 0, 0, 0, 0};
int targetState[6]  = {0, 0, 0, 0, 0, 0};

bool newTarget        = false;
bool activated        = false;
bool shouldActivate   = false;
bool shouldDeactivate = false;
bool shouldMove       = false;

void setup() {
  Serial.begin(9600);

  lights.initialize();

  pinMode(DIR_SER,    OUTPUT);
  pinMode(DIR_RCLK,   OUTPUT);
  pinMode(DIR_SRCLK,  OUTPUT);

  pinMode(STEP_SER,   OUTPUT);
  pinMode(STEP_RCLK,  OUTPUT);
  pinMode(STEP_SRCLK, OUTPUT);

  pinMode(A0, INPUT_PULLDOWN);
  pinMode(A1, INPUT_PULLDOWN);
  pinMode(A2, INPUT_PULLDOWN);
  pinMode(A3, INPUT_PULLDOWN);
  pinMode(A4, INPUT_PULLDOWN);
  pinMode(A5, INPUT_PULLDOWN);

  Particle.function("trigger", trigger);

  deactivateSteppers();
  calibrate();
}

void calibrate() {
  activateSteppers();

  while(uncalibrated()) {
    for (int i = 0; i < 6; i++) {
      if (!calibrated[i]) {
        if (buttonPressed(i)) {
          calibrated[i]   = true;
          currentState[i] = 0;
          targetState[i]  = map(5, 0, 100, 0, STEP_COUNT);
        } else {
          targetState[i] -= 1;
        }
      }
    }

    determineDirections();
    moveTowardsTarget();
  }
}

bool uncalibrated() {
  for (int i = 0; i < 6; i++) {
    if (!calibrated[i]) return true;
  }

  return false;
}

bool buttonPressed(int i) {
  return digitalRead(A0 + i);
}

int trigger(String input) {
  newTarget = true;

  int checksum  = 0;
  int index     = 0;
  int lastIndex = 0;
  int value;

  for (int i = 0; i < 6; i++) {
    int index = input.indexOf(",", lastIndex);

    if (index == -1) {
      if (i != 5) {
        // abandon ship; reset target state to current state
        for (int i = 0; i < 6; i++) {
          targetState[i] = currentState[i];
        }
        return -1;
      }
      index = input.length();
    }

    value           = atoi(input.substring(lastIndex, index));
    value           = max(0,   value);
    value           = min(100, value);
    targetState[i]  = map(value, 0, 100, 0, STEP_COUNT);
    checksum       += value;

    lastIndex = index + 1;
  }

  return checksum;
}

void loop() {
  determineState();
  display();
}

void determineState() {
  for (int i = 0; i < 6; i++) {
    if (buttonPressed(i)) {
      currentState[i] = 0;
    }
  }

  if (targetDifferent()) {
    shouldMove = true;

    if (newTarget) {
      newTarget      = false;
      shouldActivate = true;
    }
  } else {
    if (activated) {
      shouldDeactivate = true;
    }
  }

  lights.updatePositions(currentState);
  lights.updateState();
}

void display() {
  if (shouldActivate) {
    shouldActivate = false;
    activateSteppers();
    determineDirections();
  }

  if (shouldMove) {
    shouldMove = false;
    moveTowardsTarget();
  }

  if (shouldDeactivate) {
    shouldDeactivate = false;
    deactivateSteppers();
  }

  lights.show();
}

bool targetDifferent() {
  for (int i = 0; i < 6; i++) {
    if (currentState[i] != targetState[i]) {
      return true;
    }
  }

  return false;
}

void determineDirections() {
  for (int i = 0; i < 6; i++) {
    if (currentState[i] != targetState[i]) {
      setDir(i, currentState[i] > targetState[i]);
    }
  }

  writeDirRegister();
}

void moveTowardsTarget() {
  for (int i = 0; i < 6; i++) {
    if (currentState[i] != targetState[i]) {
      if (currentState[i] < targetState[i]) {
        currentState[i] += 1;
      } else {
        currentState[i] -= 1;
      }

      setStep(i, HIGH);
    } else {
      setStep(i, LOW);
    }
  }

  writeStepRegister();
}

void activateSteppers() {
  activated = true;

  setDir(7, HIGH);
  writeDirRegister();
}

void deactivateSteppers() {
  activated = false;

  setDir(7, LOW);
  writeDirRegister();
}

void setDir(int index, int value) {
  dirRegister[index] = value;
}

void setStep(int index, int value) {
  stepRegister[index] = value;
}

void writeDirRegister() {
  digitalWrite(DIR_RCLK, LOW);

  for(int i = 7; i >=  0; i--) {
    digitalWrite(DIR_SRCLK, LOW);
    digitalWrite(DIR_SER, dirRegister[i]);
    digitalWrite(DIR_SRCLK, HIGH);
  }
  digitalWrite(DIR_RCLK, HIGH);
}

void writeStepRegister() {
  // Write out contents of stepRegister[]
  digitalWrite(STEP_RCLK, LOW);
  for(int i = 7; i >=  0; i--) {
    digitalWrite(STEP_SRCLK, LOW);
    digitalWrite(STEP_SER, stepRegister[i]);
    digitalWrite(STEP_SRCLK, HIGH);
  }
  digitalWrite(STEP_RCLK, HIGH);

  // Write out LOW
  digitalWrite(STEP_RCLK, LOW);
  for(int i = 7; i >=  0; i--) {
    digitalWrite(STEP_SRCLK, LOW);
    digitalWrite(STEP_SER, LOW);
    digitalWrite(STEP_SRCLK, HIGH);
  }
  digitalWrite(STEP_RCLK, HIGH);
}

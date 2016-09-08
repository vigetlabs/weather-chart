#include "neopixel/neopixel.h"
#include "math.h"

#define STEP_COUNT (4450 * 4)

bool rainbowMode = false;

int LED_PIN   = D0;
int LED_COUNT = 149;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, WS2812B);

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

  strip.begin();
  strip.setBrightness(255);
  strip.show();

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
  Particle.function("rainbow", rainbow);

  deactivateSteppers();
  calibrate();
}

int rainbow(String command) {
  rainbowMode = !rainbowMode;

  if (rainbowMode) {
    return 1;
  } else {
    return 0;
  }
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
        setTargetToCurrent();
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

void setTargetToCurrent() {
  for (int i = 0; i < 6; i++) {
    targetState[i] = currentState[i];
  }
}

void loop() {
  determineState();
  display();
}

void determineState() {
  if (rainbowMode) return;

  for (int i = 0; i < 6; i++) {
    if (buttonPressed(i)) {
      currentState[i] = 0;
    }
  }

  calculateLedPositions();
  if (ledsDifferent()) {
    shouldShowLeds = true;
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
}

void display() {
  if (rainbowMode) {
    runRainbow(50);
    return;
  }

  if (shouldActivate) {
    shouldActivate = false;
    activateSteppers();
    determineDirections();
  }

  if (shouldMove) {
    shouldMove = false;
    moveTowardsTarget();
  }

  if (shouldShowLeds) {
    shouldShowLeds = false;
    showLeds();
  }

  if (shouldDeactivate) {
    shouldDeactivate = false;
    deactivateSteppers();
  }
}

bool ledsDifferent() {
  for (int i = 0; i < 6; i++) {
    if (currentLedState[i] != targetLedState[i]) {
      return true;
    }
  }

  return false;
}


void calculateLedPositions() {
  float tracker = 0.5;

  for (int i = 0; i < 5; i++) {
    int x = map(currentState[i],   0, STEP_COUNT, 0, 100);
    int y = map(currentState[i+1], 0, STEP_COUNT, 0, 100);

    float gap = pow(144 + (0.0441 * pow(x - y, 2)), 0.5);
    targetLedState[i] = tracker;

    tracker += gap;
  }

  targetLedState[5] = tracker;
}

void showLeds() {
  for (int i = 0; i < LED_COUNT; i++) {
    if (i > targetLedState[5]) {
      strip.setPixelColor(i, 0, 0, 0);
    } else {
      strip.setPixelColor(i, 0, 100, 100);
    }
  }

  for (int i = 0; i < 6; i++) {
    strip.setPixelColor(targetLedState[i], 255, 0, 0);
    currentLedState[i] = targetLedState[i];
  }

  strip.show();
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

void runRainbow(uint8_t wait) {
  uint16_t i, j;
  int length = currentLedState[5];

  for(j = 0; j < length; j++) {
    for(i = LED_COUNT; i >= LED_COUNT - length ; i--) {
      byte position = map((i+j) % length, 0, length, 0, 255);
      uint32_t color;

      if(position < 85) {
        color = strip.Color(position * 3, 255 - position * 3, 0);
      } else if(position < 170) {
        position -= 85;
        color = strip.Color(255 - position * 3, 0, position * 3);
      } else {
        position -= 170;
        color = strip.Color(0, position * 3, 255 - position * 3);
      }
      strip.setPixelColor(i, color);
    }
    strip.show();
    delay(wait);
  }
}

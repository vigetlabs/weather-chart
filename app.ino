int DIR_SER   = D2;
int DIR_RCLK  = D3;
int DIR_SRCLK = D4;

int STEP_SER   = D5;
int STEP_RCLK  = D6;
int STEP_SRCLK = D7;

boolean dirRegister[8];
boolean stepRegister[8];

int currentState[6] = {0, 0, 0, 0, 0, 0};
int targetState[6]  = {0, 0, 0, 0, 0, 0};

bool newTarget        = false;
bool activated        = false;
bool shouldActivate   = false;
bool shouldDeactivate = false;
bool shouldMove       = false;

void setup() {
  pinMode(DIR_SER,    OUTPUT);
  pinMode(DIR_RCLK,   OUTPUT);
  pinMode(DIR_SRCLK,  OUTPUT);

  pinMode(STEP_SER,   OUTPUT);
  pinMode(STEP_RCLK,  OUTPUT);
  pinMode(STEP_SRCLK, OUTPUT);

  Particle.function("trigger", trigger);
}

int trigger(String command) {
  newTarget = true;

  if (targetState[0] == 0) {
    for (int i = 0; i < 6; i++) {
      targetState[i] = (i + 1) * 300;
    }
  } else {
    for (int i = 0; i < 6; i++) {
      targetState[i] = 0;
    }
  }

  return 1;
}

void loop() {
  determineState();
  display();
}

void determineState() {
  if (targetDifferent()) {
    shouldMove = true;

    if (newTarget) {
      newTarget      = false;
      shouldActivate = true;
      activated      = true;
    }
  } else {
    if (activated) {
      activated        = false;
      shouldDeactivate = true;
    }
  }
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
      setDir(i, currentState[i] < targetState[i]);
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
  setDir(6, HIGH);
  writeDirRegister();
}

void deactivateSteppers() {
  setDir(6, LOW);
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
  delayMicroseconds(400);

  // Write out LOW
  digitalWrite(STEP_RCLK, LOW);
  for(int i = 7; i >=  0; i--) {
    digitalWrite(STEP_SRCLK, LOW);
    digitalWrite(STEP_SER, LOW);
    digitalWrite(STEP_SRCLK, HIGH);
  }
  digitalWrite(STEP_RCLK, HIGH);
  delayMicroseconds(400);
}

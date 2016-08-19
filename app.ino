int DIR_SER   = D2;
int DIR_RCLK  = D3;
int DIR_SRCLK = D4;

int STEP_SER   = D5;
int STEP_RCLK  = D6;
int STEP_SRCLK = D7;

boolean dirRegister[8];
boolean stepRegister[8];

int currentState[6] {0, 0, 0, 0, 0, 0};
int targetState[6]  {0, 0, 0, 0, 0, 0};

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
  if (targetState[0] == 0) {
    for (int i = 0; i < 6; i++) {
      targetState[i] = 1000;
    }
  } else {
    for (int i = 0; i < 6; i++) {
      targetState[i] = 0;
    }
  }

  return 1;
}

void loop() {
  if (targetDifferent()) {
    moveTowardsTarget();
  } else {
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

void moveTowardsTarget() {
  activateSteppers();

  for (int i = 0; i < 6; i++) {
    if (currentState[i] != targetState[i]) {
      bool direction = currentState[i] < targetState[i];
      if (direction == true) {
        currentState[i] = currentState[i] + 1;
      } else {
        currentState[i] = currentState[i] - 1;
      }
      setDir(i, direction);
      setStep(i, HIGH);
    } else {
      setStep(i, LOW);
    }
  }

  writeDirRegister();
  writeStepRegister();
}

// void run(int direction) {
//   activateSteppers();
//
//   setDir(0, direction);
//   setDir(1, direction);
//   setDir(2, direction);
//   setDir(3, direction);
//   setDir(4, direction);
//   setDir(5, direction);
//   writeDirRegister();
//
//   for (int i = 0; i < 1000; i++) {
//     setStep(0, HIGH);
//     setStep(1, HIGH);
//     setStep(2, HIGH);
//     setStep(3, HIGH);
//     setStep(4, HIGH);
//     setStep(5, HIGH);
//     writeStepRegister();
//     delayMicroseconds(400);
//
//     setStep(0, LOW);
//     setStep(1, LOW);
//     setStep(2, LOW);
//     setStep(3, LOW);
//     setStep(4, LOW);
//     setStep(5, LOW);
//     writeStepRegister();
//     delayMicroseconds(400);
//   }
//
//   deactivateSteppers();
// }


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

int DIR_SER   = D2;
int DIR_RCLK  = D3;
int DIR_SRCLK = D4;

int STEP_SER   = D5;
int STEP_RCLK  = D6;
int STEP_SRCLK = D7;

boolean dirRegister[8];
boolean stepRegister[8];

void setup() {
  pinMode(DIR_SER,    OUTPUT);
  pinMode(DIR_RCLK,   OUTPUT);
  pinMode(DIR_SRCLK,  OUTPUT);

  pinMode(STEP_SER,   OUTPUT);
  pinMode(STEP_RCLK,  OUTPUT);
  pinMode(STEP_SRCLK, OUTPUT);
}

void loop() {
  run(HIGH);
  delay(2000);
  run(LOW);
  delay(2000);
}

void run(int direction) {
  setDir(0, direction);
  setDir(1, direction);
  setDir(2, direction);
  setDir(3, direction);
  setDir(4, direction);
  setDir(5, direction);
  writeDirRegister();

  for (int i = 0; i < 1000; i++) {
    setStep(0, HIGH);
    setStep(1, HIGH);
    setStep(2, HIGH);
    setStep(3, HIGH);
    setStep(4, HIGH);
    setStep(5, HIGH);
    writeStepRegister();
    delayMicroseconds(400);

    setStep(0, LOW);
    setStep(1, LOW);
    setStep(2, LOW);
    setStep(3, LOW);
    setStep(4, LOW);
    setStep(5, LOW);
    writeStepRegister();
    delayMicroseconds(400);
  }
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

    int val = dirRegister[i];

    digitalWrite(DIR_SER, val);
    digitalWrite(DIR_SRCLK, HIGH);
  }
  digitalWrite(DIR_RCLK, HIGH);
}

void writeStepRegister() {
  digitalWrite(STEP_RCLK, LOW);

  for(int i = 7; i >=  0; i--) {
    digitalWrite(STEP_SRCLK, LOW);

    int val = stepRegister[i];

    digitalWrite(STEP_SER, val);
    digitalWrite(STEP_SRCLK, HIGH);
  }
  digitalWrite(STEP_RCLK, HIGH);
}

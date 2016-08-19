int SER_Pin   = D0;  //pin 14 on the 75HC595
int RCLK_Pin  = D1;  //pin 12 on the 75HC595
int SRCLK_Pin = D2; //pin 11 on the 75HC595

//How many of the shift registers - change this
#define number_of_74hc595s 2

//do not touch
#define numOfRegisterPins number_of_74hc595s * 8

boolean registers[numOfRegisterPins];

void setup(){
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);

  //reset all register pins
  clearRegisters();
  writeRegisters();
}

//set all register pins to LOW
void clearRegisters(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
     registers[i] = LOW;
  }
}

//Set and display registers
//Only call AFTER all values are set how you would like (slow otherwise)
void writeRegisters(){
  digitalWrite(RCLK_Pin, LOW);

  for(int i = numOfRegisterPins - 1; i >=  0; i--){
    digitalWrite(SRCLK_Pin, LOW);

    int val = registers[i];

    digitalWrite(SER_Pin, val);
    digitalWrite(SRCLK_Pin, HIGH);

  }
  digitalWrite(RCLK_Pin, HIGH);
}

//set an individual pin HIGH or LOW
void setRegisterPin(int index, int value){
  registers[index] = value;
}


void loop(){
  setRegisterPin(0,  HIGH);
  setRegisterPin(1,  LOW);
  setRegisterPin(2,  HIGH);
  setRegisterPin(3,  LOW);
  setRegisterPin(4,  HIGH);
  setRegisterPin(5,  LOW);
  setRegisterPin(6,  HIGH);
  setRegisterPin(7,  LOW);
  setRegisterPin(8,  HIGH);
  setRegisterPin(9,  LOW);
  setRegisterPin(10, HIGH);
  setRegisterPin(11, LOW);
  setRegisterPin(12, HIGH);
  setRegisterPin(13, LOW);
  setRegisterPin(14, HIGH);
  setRegisterPin(15, LOW);

  writeRegisters();  //MUST BE CALLED TO DISPLAY CHANGES
  //Only call once after the values are set how you need.
}

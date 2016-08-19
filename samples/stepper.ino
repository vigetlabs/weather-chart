//l1 on big setup
//l2 on small setup

int dp0 = A0;
int sp0 = D0;
int dp1 = A1;
int sp1 = D1;
int dp2 = A2;
int sp2 = D2;
int dp3 = A3;
int sp3 = D3;
int dp4 = A4;
int sp4 = D4;
int dp5 = A5;
int sp5 = D5;

void setup() {
  pinMode(dp0, OUTPUT);
  pinMode(sp0, OUTPUT);
  pinMode(dp1, OUTPUT);
  pinMode(sp1, OUTPUT);
  pinMode(dp2, OUTPUT);
  pinMode(sp2, OUTPUT);
  pinMode(dp3, OUTPUT);
  pinMode(sp3, OUTPUT);
  pinMode(dp4, OUTPUT);
  pinMode(sp4, OUTPUT);
  pinMode(dp5, OUTPUT);
  pinMode(sp5, OUTPUT);
}

void step(boolean dir,int steps){
  digitalWrite(dp0,dir);
  digitalWrite(dp1,dir);
  digitalWrite(dp2,dir);
  digitalWrite(dp3,dir);
  digitalWrite(dp4,dir);
  digitalWrite(dp5,dir);

  delay(50);

  for(int i=0;i<steps;i++){
    digitalWrite(sp0, HIGH);
    digitalWrite(sp1, HIGH);
    digitalWrite(sp2, HIGH);
    digitalWrite(sp3, HIGH);
    digitalWrite(sp4, HIGH);
    digitalWrite(sp5, HIGH);
    delayMicroseconds(400);

    digitalWrite(sp0, LOW);
    digitalWrite(sp1, LOW);
    digitalWrite(sp2, LOW);
    digitalWrite(sp3, LOW);
    digitalWrite(sp4, LOW);
    digitalWrite(sp5, LOW);
    delayMicroseconds(400);
  }
}

void loop(){
  step(true,3000);
  delay(2000);
  step(false,3000);
  delay(2000);
}

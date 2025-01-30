const int ledin1 = 2;
const int ledin2 = 3;
const int ledin3 = 4;

const int ledout1 = 8;
const int ledout2 = 9;
const int ledout3 = 10;

//buttons
const int buttonA = 5;
const int buttonB = 6;
const int buttonC = 7;

const int button1 = 11;
const int button2 = 12;
const int button3 = 13;

// potentiometer

const int potent = A0;

void updateMatrix(bool vala, bool valb, bool valc, bool val1, bool val2, bool val3) {
  vala ? digitalWrite(ledin1, HIGH) : digitalWrite(ledin1, LOW);
  valb ? digitalWrite(ledin2, HIGH) : digitalWrite(ledin2, LOW);
  valc ? digitalWrite(ledin3, HIGH) : digitalWrite(ledin3, LOW);

  val1 ? digitalWrite(ledout1, HIGH) : digitalWrite(ledout1, LOW);
  val2 ? digitalWrite(ledout2, HIGH) : digitalWrite(ledout2, LOW);
  val3 ? digitalWrite(ledout3, HIGH) : digitalWrite(ledout3, LOW);
}



void setup() {
  Serial.begin(9600);
  //Led config
  pinMode(ledin1, OUTPUT);
  pinMode(ledin2, OUTPUT);
  pinMode(ledin3, OUTPUT);

  pinMode(ledout1, OUTPUT);
  pinMode(ledout2, OUTPUT);
  pinMode(ledout3, OUTPUT);

  //Button config
  pinMode(buttonA, OUTPUT);
  pinMode(buttonB, OUTPUT);
  pinMode(buttonC, OUTPUT);

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);

  digitalWrite(buttonA, HIGH);
  digitalWrite(buttonB, HIGH);
  digitalWrite(buttonC, HIGH);


}

void readButtons(){
  
}

char = 0x01 << 2;  // 0000 0100 

void animateCross(int speed) {
  updateMatrix(1, 0, 0, 0, 1, 1);
  delay(speed);
  updateMatrix(0, 1, 0, 1, 0, 1);
  delay(speed);
  updateMatrix(0, 0, 1, 1, 1, 0);
  delay(speed);
  updateMatrix(0, 0, 1, 0, 1, 1);
  delay(speed);
  updateMatrix(1, 0, 0, 1, 1, 0);
  delay(speed);
}

void animateDiamond(int speed) {
  updateMatrix(0, 1, 0, 0, 1, 1);
  delay(speed);
  updateMatrix(0, 0, 1, 1, 0, 1);
  delay(speed);
  updateMatrix(1, 0, 0, 1, 0, 1);
  delay(speed);
  updateMatrix(0, 1, 0, 1, 1, 0);
  delay(speed);
}


void loop() {

  int i = analogRead(potent);

  animateCross(i);
  animateDiamond(i);

}

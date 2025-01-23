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

bool vala = false; 
bool valb = false;
bool valc = false;

bool val1 = false;
bool val2 = false;
bool val3 = false;

void updateMatrix(bool vala, bool valb , bool valc, bool val1, bool val2, bool val3){
  vala ? digitalWrite(ledin1, HIGH) : digitalWrite(ledin1, LOW);
  valb ? digitalWrite(ledin2, HIGH) : digitalWrite(ledin2, LOW);
  valc ? digitalWrite(ledin3, HIGH) : digitalWrite(ledin3, LOW);

  val1 ? digitalWrite(ledout1, HIGH) : digitalWrite(ledout1, LOW);
  val2 ? digitalWrite(ledout2, HIGH) : digitalWrite(ledout2, LOW);
  val3 ? digitalWrite(ledout3, HIGH) : digitalWrite(ledout3, LOW);
}



void setup() {
  //Led config
pinMode(ledin1, OUTPUT);
pinMode(ledin2, OUTPUT);
pinMode(ledin3, OUTPUT);

pinMode(ledout1, OUTPUT);
pinMode(ledout2, OUTPUT);
pinMode(ledout3, OUTPUT);

digitalWrite(ledin1, HIGH);
digitalWrite(ledin2, HIGH);
digitalWrite(ledin3, HIGH);

digitalWrite(ledout1, LOW);
digitalWrite(ledout2, HIGH);
digitalWrite(ledout3, HIGH);

 //Button config
 pinMode(buttonA, INPUT);
 pinMode(buttonB, INPUT);
 pinMode(buttonC, INPUT);

 pinMode(button1, INPUT);
 pinMode(button2, INPUT);
 pinMode(button3, INPUT);

updateMatrix(1,1,1,0,1,1);


}



void loop() {

for (int i=50; 1>0; ){
  updateMatrix(1,0,0,0,1,1); 
  delay(i);
  updateMatrix(0,1,0,1,0,1);
  delay(i);
  updateMatrix(0,0,1,1,1,0);
  delay(i);
  updateMatrix(0,0,1,0,1,1);
  delay(i);
  updateMatrix(1,0,0,1,1,0);
  delay(i);
  if (i > 1){
    i--;
  }
}




}

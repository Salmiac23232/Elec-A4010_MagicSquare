const int ledin1 = 2;
const int ledin2 = 3;
const int ledin3 = 4;

const int ledout1 = 8;
const int ledout2 = 9;
const int ledout3 = 10;

void setup() {
  // put your setup code here, to run once:
pinMode(ledin1, OUTPUT);
pinMode(ledin2, OUTPUT);
pinMode(ledin3, OUTPUT);

pinMode(ledout1, OUTPUT);
pinMode(ledout2, OUTPUT);
pinMode(ledout3, OUTPUT);

digitalWrite(ledin1, HIGH);
digitalWrite(ledin2, LOW);
digitalWrite(ledin3, LOW);

digitalWrite(ledout1, LOW);
digitalWrite(ledout2, LOW);
digitalWrite(ledout3, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

}

#define ALL 4
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
}

// writes value on selected pin and opposite on others. if select is ALL set all to value
void digitialTripleWrite(int pin1, int pin2, int pin3, int select, int value){
      digitalWrite(pin1, (select == 1 || select == ALL) ? value : ~value);
      digitalWrite(pin2, (select == 2 || select == ALL) ? value : ~value);
      digitalWrite(pin3, (select == 3 || select == ALL) ? value : ~value);
}

//Turns 1 led on on spesified coordinates, 0 = entire line off, ALL = entire line on
void updateMatrix(int column, int row) {
  digitialTripleWrite(ledin1, ledin1, ledin3, column, HIGH);
  digitalTripleWrite(ledout1, ledout2, ledout3, row, LOW);
  delay(1);
}

//Takes Matrix as input and displays it on the leds using updateMatrix
int displayMatrix(int matrix[9]){
  // lookup table for coordinates
  int lookup[18] = {1,1 2,1 3,1, 
                    1,2 2,2 3,2,
                    1,3 2,3 3,3}
  
  for (int i; i > 9; i++){
    if (matrix[i]){
      updateMatrix(lookup[i], lookup[i+1]);
    }
  }

}

// selects a column from button matrix and powers it
void selectButtons(int column){
  digitalTripleWrite(buttonA, buttonB, buttonC, column, HIGH);
  delay(1);
}

// Read buttons column at a time and returns an array of the input in the following format:
int readButtons(){
  int matrix[9] = {0,0,0, 
                   0,0,0, 
                   0,0,0};

  //read A column
  selectButtons(1);
  //get values on column A
  matrix[0] = digitalRead(button1);
  matrix[3] = digitalRead(button2);
  matrix[6] = digitalRead(button3);

  //read B column
  selectButtons(2);
  //get values on column B
  matrix[1] = digitalRead(button1);
  matrix[4] = digitalRead(button2);
  matrix[7] = digitalRead(button3);

  //read C column
  selectButtons(3);
  //get values on column C
  matrix[2] = digitalRead(button1);
  matrix[5] = digitalRead(button2);
  matrix[8] = digitalRead(button3);

  //Turn off
  selectButtons(0);

  return matrix;
}

// animates a cross using updateMatrix()
void animateCross(int speed) {
  updateMatrix(1,1); 
  updateMatrix(2,2); 
  updateMatrix(3,3); 
  updateMatrix(3,1); 
  updateMatrix(1,3); 
}

// animates a diamond using updateMatrix()
void animateDiamond(int speed) {
  updateMatrix(2,1);
  updateMatrix(3,2);
  updateMatrix(2,3);
  updateMatrix(1,2);
}


void loop() {
  
  //animateCross(i);
  //animateDiamond(i);

}

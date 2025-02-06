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

// Arrays
int matrix[9] = {0,0,0, 
                 0,0,0, 
                 0,0,0};

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
void digitalTripleWrite(int pin1, int pin2, int pin3, int select, int value){
      digitalWrite(pin1, ((select == 1 || select == ALL) ? value : !value));
      digitalWrite(pin2, ((select == 2 || select == ALL) ? value : !value));
      digitalWrite(pin3, ((select == 3 || select == ALL) ? value : !value));
}

//Turns 1 led on on spesified coordinates, 0 = entire line off, ALL = entire line on
void updateMatrix(int column, int row) {
  digitalTripleWrite(ledin1, ledin2, ledin3, column, HIGH);
  digitalTripleWrite(ledout1, ledout2, ledout3, row, LOW);
}

//Takes Matrix as input and displays it on the leds using updateMatrix
int displayMatrix(int matrix[9]){
  // lookup table for coordinates
  int lookup[18] = {1,1, 2,1, 3,1, 
                    1,2, 2,2, 3,2,
                    1,3, 2,3, 3,3};
  
  for (int i = 0; i < 9; i++){
    if (matrix[i]){
      updateMatrix(lookup[2*i], lookup[2*i+1]);
      delay(100);
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
  for (int i = 0; i < 9; i++){
    matrix[i] = 0;
  }

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

  /*for (int i=0; i<9; i++){
    Serial.print(matrix[i]);
    if(i == 2 || i == 5 || i == 8 ){
      Serial.print("\n");
    }
  }

  Serial.print("\n");
  delay(1);*/
  return matrix;
}

// animates a cross using updateMatrix()
void animateCross(int speed) {
  updateMatrix(1,1); 
  delay(speed);
  updateMatrix(2,2);
  delay(speed); 
  updateMatrix(3,3);
  delay(speed); 
  updateMatrix(3,1);
  delay(speed); 
  updateMatrix(1,3);
  delay(speed); 
}

// animates a diamond using updateMatrix()
void animateDiamond(int speed) {
  updateMatrix(2,1);
  delay(speed);
  updateMatrix(3,2);
  delay(speed);
  updateMatrix(2,3);
  delay(speed);
  updateMatrix(1,2);
  delay(speed);
}


void loop() {

  displayMatrix(readButtons());

  //animateCross(1000);
  //animateDiamond(1000);

}

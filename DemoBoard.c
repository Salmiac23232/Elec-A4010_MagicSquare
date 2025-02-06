#include <FastLED_NeoPixel.h> 
#define ALL 4
#define DATA_PIN 0
#define NUM_LEDS 9
#define BRIGHTNESS 255

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


// Matricies
int leds[9] = {0};
int inputs[9] = {0};
int prev_inputs[9] = {0};


FastLED_NeoPixel<NUM_LEDS, DATA_PIN, NEO_GRB> strip;  
void setup() {
	strip.begin();  // initialize strip (required!)
	strip.setBrightness(BRIGHTNESS);

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
  updateMatrix(0,0);
  // lookup table for coordinates
  int lookup[18] = {1,1, 2,1, 3,1, 
                    1,2, 2,2, 3,2,
                    1,3, 2,3, 3,3};
  
  for (int i = 0; i < 9; i++){
    if (matrix[i]){
      updateMatrix(lookup[2*i], lookup[2*i+1]);
      delay(100);
      updateMatrix(0,0);
    }
    else {
      delay(100);
    }
  } 
 
}

// selects a column from button matrix and powers it
void selectButtons(int column){
  digitalTripleWrite(buttonA, buttonB, buttonC, column, HIGH);
}

// Read buttons column at a time and returns an array of the input in the following format:
int readButtons(){
  for (int i = 0; i < 9; i++){
    inputs[i] = 0;
  }

  //read A column
  selectButtons(1);
  //get values on column A
  inputs[0] = digitalRead(button1);
  inputs[3] = digitalRead(button2);
  inputs[6] = digitalRead(button3);

  //read B column
  selectButtons(2);
  //get values on column B
  inputs[1] = digitalRead(button1);
  inputs[4] = digitalRead(button2);
  inputs[7] = digitalRead(button3);

  //read C column
  selectButtons(3);
  //get values on column C
  inputs[2] = digitalRead(button1);
  inputs[5] = digitalRead(button2);
  inputs[8] = digitalRead(button3);

  //Turn off
  selectButtons(0);
  return inputs;
}

//Toggles the leds matrix by comparing the previous inputs to the button being pressed.
int toggleButtons(){
  for (int i = 0; i<9; i++){
    if (inputs[i] > prev_inputs[i]){
      leds[i] = !leds[i];
    }
    prev_inputs[i] = inputs[i];
  }
  
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

  updateMatrix(ALL,ALL);

  colorWipe(strip.Color(255, 0, 0), 10);  // Green
  colorWipe(strip.Color(255, 255, 0), 10);  //Yellow
  colorWipe(strip.Color(0, 255, 0), 10);  // Red
  colorWipe(strip.Color(0, 255, 255), 10);  // Purple
  colorWipe(strip.Color(0, 0, 255), 10);  // Blue
  colorWipe(strip.Color(255, 0, 255), 10);  // Cyan


  //readButtons();
  //toggleButtons();
  //displayMatrix(leds);

  //animateCross(1);
  //animateDiamond(1000);

}

void colorWipe(uint32_t color, unsigned long wait) {
	for (unsigned int i = 0; i < strip.numPixels(); i++) {
		strip.setPixelColor(i, color);
		strip.show();
		delay(wait);
	}
}

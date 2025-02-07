#include <FastLED_NeoPixel.h> 
#define ALL 4
#define DATA_PIN 2
#define NUM_LEDS 9
#define BRIGHTNESS 50

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


// Matricies
uint32_t colors[9] = {0}; // This is the color matrix that the displayMatrix uses. 
int inputs[9] = {0};
int prev_inputs[9] = {0};

// Fast Led Setup and Colors
FastLED_NeoPixel<NUM_LEDS, DATA_PIN, NEO_GRB> strip;  

int turn = 1;


void setup() {
  Serial.begin(9600);
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

//Takes Leds as input and displays it on the leds using updateMatrix
int displayMatrix(uint32_t color[9]){
  
  for (int i = 0; i < 9; i++){
    strip.setPixelColor(i, color[i]);
  } 
 	strip.show();
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

//Toggles to a given color the leds matrix by comparing the previous inputs to the button being pressed.
// returns how many times a change has happened. 
int toggleButtons(uint32_t color){
  int changes = 0;
  for (int i = 0; i<9; i++){
    if (inputs[i] > prev_inputs[i]){
      colors[i] = (!colors[i]*color); // Toggle value
      changes++; // count as 1 change made
    }
    prev_inputs[i] = inputs[i];
  }
  return changes;
}
  


void loop() {
  
  readButtons();
  toggleButtons(strip.Color(0, 255, 0));  
  displayMatrix(colors);

}



void colorWipe(uint32_t color, unsigned long wait) {
	for (unsigned int i = 0; i < strip.numPixels(); i++) {
		strip.setPixelColor(i, color);
		strip.show();
		delay(wait);
	}
}

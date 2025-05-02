#include <LiquidCrystal_I2C.h>
#include <FastLED_NeoPixel.h>
#include <pitches.h>
#define ALL 4
#define DATA_PIN 2
#define NUM_LEDS 9
#define BRIGHTNESS 100

//buttons
const int buttonA = 8;
const int buttonB = 9;
const int buttonC = 10;

const int button1 = 11;
const int button2 = 12;
const int button3 = 13;

//Speaker
const int speaker = 3;

//Songs

struct song{
  int* chart;
  int* melody;
  int* noteDurations;
  int size;
};
// Level 1

int chart[] = {
  1, 3, 3, 7, 1, 0, 4, 2
};

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

struct song tutorial = {chart, melody, noteDurations, 8};

// Level 2
int chart2[] = {
  1,1,2,3,3,5,7,7,8,9,9,5,1,3,9,7,1,5,5
};

int melody2[] = {
  NOTE_E4, NOTE_B3, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_A3, NOTE_C4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_B3, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4, NOTE_A3, NOTE_A3
};

int noteDurations2[] = {
  4,8,8,4,8,8,4,8,8,4,8,8,4,6,4,4,4,4,4
};

struct song tetris = {chart2, melody2, noteDurations2, 19};

//LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);


// Matricies
uint32_t colors[9] = { 0 };  // This is the color matrix that the displayMatrix uses.
int inputs[9] = { 0 };
int prev_inputs[9] = { 0 };

// Fast Led Setup and Colors
FastLED_NeoPixel<NUM_LEDS, DATA_PIN, NEO_GRB> strip;

//Menu
int inMenu = 1;
int menu_Select[9] = { 0 };
int Game = 0;



void setup() {

  Serial.begin(9600);
  strip.begin();  // initialize strip (required!)
  strip.setBrightness(BRIGHTNESS);

  lcd.init();
  lcd.clear();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3, 1);
  lcd.print("Booting UP");
  colorWipe(strip.Color(255, 0, 0), 20);
  colorWipe(strip.Color(255, 255, 0), 20);
  colorWipe(strip.Color(0, 255, 0), 20);
  colorWipe(strip.Color(0, 255, 255), 20);
  colorWipe(strip.Color(255, 255, 255), 20);

  tone(speaker, 262, 250);  //C
  delay(250);
  lcd.print(".");
  tone(speaker, 330, 250);  //E
  delay(250);
  lcd.print(".");
  tone(speaker, 392, 250);  //G
  delay(250);
  lcd.print(".");
  delay(250);


  lcd.clear();
  lcd.setCursor(3, 1);
  lcd.print("Select Game: ");
  colorWipe(strip.Color(0, 255, 0), 20);

  //Button config
  pinMode(buttonA, OUTPUT);
  pinMode(buttonB, OUTPUT);
  pinMode(buttonC, OUTPUT);

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
}

// writes value on selected pin and opposite on others. if select is ALL set all to value
void digitalTripleWrite(int pin1, int pin2, int pin3, int select, int value) {
  digitalWrite(pin1, ((select == 1 || select == ALL) ? value : !value));
  digitalWrite(pin2, ((select == 2 || select == ALL) ? value : !value));
  digitalWrite(pin3, ((select == 3 || select == ALL) ? value : !value));
}

//Takes Leds as input and displays it on the leds using updateMatrix
int displayMatrix(uint32_t color[9]) {

  for (int i = 0; i < 9; i++) {
    strip.setPixelColor(i, color[i]);
  }
  strip.show();
}

// selects a column from button matrix and powers it
void selectButtons(int column) {
  digitalTripleWrite(buttonA, buttonB, buttonC, column, HIGH);
}

// Read buttons column at a time and returns an array of the input in the following format:
int readButtons() {
  for (int i = 0; i < 9; i++) {
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
int toggleArray(uint32_t array[9], uint32_t value) {
  int changes = 0;
  for (int i = 0; i < 9; i++) {
    if (inputs[i] > prev_inputs[i]) {
      array[i] = (!array[i] * value);  // Toggle value
      changes++;                       // count as 1 change made
    }
    prev_inputs[i] = inputs[i];
  }
  return changes;
}

//Gets one input form buttons
void singleInput(int array[9]) {
  int set = 0;
  for (int i = 0; i < 9; i++) {
    if (inputs[i] > prev_inputs[i]) {
      // if there is a button push, initialize array to zero
      for (int j = 0; j < 9; j++) {
        array[j] = 0;
      }

      // add 1 if a set hasnt been done to others yet
      if (set == 0) {
        array[i]++;
        set = 1;
      }
    }

    prev_inputs[i] = inputs[i];
  }
}

// Updates the input array to select one of the buttons
void selectMenu(int array[9]) {
  int set = 0;
  for (int i = 0; i < 9; i++) {
    int prev = array[i];
    if (inputs[i] > prev_inputs[i]) {
      // if there is a button push, initialize array to zero
      for (int j = 0; j < 9; j++) {
        array[j] = 0;
      }
      // if it was previously 1 restore it
      if (prev) {
        array[i]++;
      }

      // add 1 if a set hasnt been done to others yet
      if (set == 0) {
        array[i]++;
        tone(speaker, 262, 250);
        set = 1;
      }
    }

    prev_inputs[i] = inputs[i];
  }

  for (int i = 0; i < 9; i++) {
    switch (array[i]) {
      case 0:
        strip.setPixelColor(i, strip.Color(0, 255, 0));
        break;
      case 1:
        strip.setPixelColor(i, strip.Color(255, 0, 0));
        //Display Logic
        switch (i) {
          case 0:
            lcd.setCursor(3, 2);
            lcd.print("Tic Tac Toe       ");
            Game = 1;
            break;
          case 1:
            lcd.setCursor(3, 2);
            lcd.print("Rythm Game       ");
            Game = 2;
            break;
          case 2:
            lcd.setCursor(3, 2);
            lcd.print("Simon Says      ");
            Game = 3;
            break;
          case 3:
            lcd.setCursor(3, 2);
            lcd.print("Settings      ");
            break;
          case 4:
            lcd.setCursor(3, 2);
            lcd.print("Credits    ");
            break;
          default:
            lcd.setCursor(3, 2);
            lcd.print("Coming Soon...    ");
            break;
        }
        break;
      case 2:
        array[i] = 1;
        lcd.setCursor(3, 1);
        lcd.print("             ");
        strip.setPixelColor(i, strip.Color(255, 255, 255));
        strip.show();
        delay(250);
        tone(speaker, 330, 250);  //E
        delay(250);
        tone(speaker, 392, 250);  //G
        delay(250);
        inMenu = 0;
        break;
    }
  }
  strip.show();
}


// Show the incoming Note in yellow
void telegraphNote(int placement, int duration) {
  int speed = duration / 255;
  if (placement) {
    uint32_t matrix[9] = { 0 };
    for (int i = 0; i < 255; i++){
          matrix[placement - 1] = strip.Color(i, i, 0);  // Yellow
          displayMatrix(matrix);
          delay(speed);
    }

  } else {
    uint32_t clear[9] = { 0 };
    displayMatrix(clear);
  }
}

// Listens for input for given button for a duration in milliseconds, Returns one if captured input, return 0 if no input
int listenForInput(int button, int duration) {
  unsigned long start = millis();
  if (button == 0){
    return 1;
  }

  while ((millis() - start) < duration) {

    int rythmInputs[9] = { 0 };
    readButtons();
    singleInput(rythmInputs);

    //Check inputs
    for (int i = 0; i < 9; i++){
      if (rythmInputs[i]){
        if (i == (button-1)){
          return 1;
        }
        return 0;
      }
    }
  }
  return 0;
}

void playSong(struct song* level){
        int points = 0;
        for (int thisNote = 0; thisNote < level->size; thisNote++) {
        int noteDuration = 2500 / level->noteDurations[thisNote];
        int pauseBetweenNotes = noteDuration * 1.30;
        uint32_t clear[9] = { 0 };

        telegraphNote(level->chart[thisNote], (pauseBetweenNotes / 8));
        int correct = listenForInput(level->chart[thisNote], (pauseBetweenNotes / 8)*3);

        uint32_t matrix[9] = { 0 };

        if (correct) {
          matrix[level->chart[thisNote] - 1] = strip.Color(255, 0, 0);  // Green
          displayMatrix(matrix);
          tone(speaker, level->melody[thisNote], noteDuration);
          points++;
          lcd.setCursor(7, 2);
          lcd.print(points);
        } else {
          matrix[level->chart[thisNote] - 1] = strip.Color(0, 255, 0);  // Red
          displayMatrix(matrix);
          tone(speaker, 62, noteDuration);
        }
        delay(pauseBetweenNotes / 2);
        noTone(speaker);
      }
}


void loop() {

  // After Boot Up, Select Game
  if (inMenu) {
    readButtons();
    selectMenu(menu_Select);
    return;
  }

  // What game to play
  switch (Game) {

    //No game, return to menu
    case 0:
      inMenu = 1;
      break;

    // Tic tac toe
    case 1:
      // Enter Game Logic Here

      // Demo Code Can Remove.
      readButtons();
      toggleArray(colors, strip.Color(0, 0, 255));
      displayMatrix(colors);
      break;

    // Rythm Game
    case 2:
      // Enter Game Logic Here
      readButtons();
      toggleArray(colors, strip.Color(255, 255, 0));
      displayMatrix(colors);

      lcd.clear();
      lcd.setCursor(3, 1);
      lcd.print("Tutorial");
      lcd.setCursor(0, 2);
      lcd.print("Score: ");
      playSong(&tutorial);
      lcd.setCursor(3, 1);
      lcd.clear();
      lcd.print("Next up Tetris");

      delay(4000);
      lcd.clear();
      lcd.setCursor(3, 1);
      lcd.print("Start!");
      delay(1000);
      lcd.clear();
      lcd.setCursor(3, 1);
      lcd.print("Tetris");
      lcd.setCursor(0, 2);
      lcd.print("Score: ");
      playSong(&tetris);

      delay(5000);
      lcd.clear();
      inMenu = 1;
      break;

    //Simon says
    case 3:
      //Enter Game Logic Here

      //Demo Code, Can remove.
      colorWipe(strip.Color(255, 0, 0), 20);
      colorWipe(strip.Color(255, 255, 0), 20);
      colorWipe(strip.Color(0, 255, 0), 20);
      colorWipe(strip.Color(0, 255, 255), 20);
      colorWipe(strip.Color(255, 255, 255), 20);
      break;
  }
}

void colorWipe(uint32_t color, unsigned long wait) {
  for (unsigned int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

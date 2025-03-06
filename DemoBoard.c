#include <LiquidCrystal_I2C.h>
#include <FastLED_NeoPixel.h>
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
















void loop() {

  // After Boot Up, Select Game
  if (inMenu) {
    readButtons();
    selectMenu(menu_Select);
    return;
  }

  // What game to play 
  switch (Game){

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
    break;

    //Simon says
    case 3:
    //Enter Game Logic Here


    // todo esta primera parte debe ir fuera del vodiog de switch and break 
    int valiaika = 1000; 
    int increase = 200;
    int correct_color = 0;
    int round = 0;


    void pattern(){

      uint32_t colors8 = strip.Color (255,0,0);//red
      uint32_t correct = strip.Color (0,255,0);//green

      
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, 0);  // turn off leds
      }
      
      
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, colors8); //it goes through each led and assigns color red
      }
      correct = random(0, NUM_LEDS);  // select random led to turn green
      
      strip.setPixelColor(correct, colors8);
      // here we select the correct led back to red 
      strip.show();
    }

    
    void loop (){
      // new round and pattern
      round++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Round: ");
      lcd.print(round);

      pattern();  // shows pattern
      
      bool botonPresionado = false;
      while (!botonPresionado) {
        readButtons(); //function that assigns the values to the buttons 
      
        for (int i = 0; i < 9; i++) {
          if (inputs[i] == LOW) {  
            if (i == correct) {
              tone(speaker, 523, 500);  // correct sound (C)
              botonPresionado = true;  // next round
              delay(500);  
            } else {
              tone(speaker,440, 500);  // incorrect sound  (A)
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("¡Game Over!");
              delay(1000);
              inMenu = 1;  // volver al menú principal
              return;
            }
          }
        }
      }

      valiaika = max(increase, valiaika - increase); // aca hacemos que la velocidad maxima sea 200
      // asi el juego no se vuelve imposible 
      delay(valiaika);  
      break;
    }

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

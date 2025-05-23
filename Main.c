#include <LiquidCrystal_I2C.h>
#include <FastLED_NeoPixel.h>
#include <pitches.h>
#include "Tic_Tac_Toe.h"
#define ALL 4
#define DATA_PIN 2
#define NUM_LEDS 9
#define BRIGHTNESS 200


// Buttons
const int buttonA = 8;
const int buttonB = 9;
const int buttonC = 10;

const int button1 = 11;
const int button2 = 12;
const int button3 = 13;

// Speaker
const int speaker = 3;


// Songs

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


// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);


// Matricies
uint32_t colors[9] = { 0 };   // This is the color matrix that the displayMatrix uses.
int inputs[9] = { 0 };
int prev_inputs[9] = { 0 };

// Fast Led Setup and Colors
FastLED_NeoPixel<NUM_LEDS, DATA_PIN, NEO_GRB> strip;

// Menu
int inMenu = 1;
int menu_Select[9] = { 0 };
int Game = 0;

// Colores para cada botón
uint32_t buttonColors[9] = {
  strip.Color(255, 0, 0),
  strip.Color(0, 255, 0),
  strip.Color(0, 0, 255),
  strip.Color(255, 255, 0),
  strip.Color(255, 0, 255),
  strip.Color(0, 255, 255),
  strip.Color(255, 165, 0),
  strip.Color(128, 0, 128),
  strip.Color(255, 255, 255)
};

// Simon Dice
#define MAX_SEQUENCE 100
int simonSequence[MAX_SEQUENCE];
int currentLength = 0;
int playerIndex = 0;
bool playingSimon = false;
unsigned long lastInputTime = 0;
const unsigned long inputTimeout = 5000;



// ---------- Lights Out -peli muuttujat ja funktiot ----------

// Määritellään matriisin koko (3x3)
#define NUM_ROWS 3
#define NUM_COLS 3

// Pelin tilan matriisi: true = LED päällä, false = pois päältä
bool lightsOutState[NUM_ROWS][NUM_COLS];

// Alustaa Lights Out -pelin tilan satunnaisesti
void initLightsOutGame() {
  randomSeed(analogRead(0));
  for (int r = 0; r < NUM_ROWS; r++){
    for (int c = 0; c < NUM_COLS; c++){
      lightsOutState[r][c] = (random(2) == 1);
    }
  }
}

// Togglataan yksittäistä solua, jos indeksi on validi
void toggleLightsOutCell(int row, int col) {
  if(row >= 0 && row < NUM_ROWS && col >= 0 && col < NUM_COLS) {
    lightsOutState[row][col] = !lightsOutState[row][col];
  }
}

// Kun pelaaja painaa tiettyä solua, togglataan se ja sen ylä-, ala-, vasen- ja oikeanpuoleiset naapurit.
void toggleLightsOut(int row, int col) {
  toggleLightsOutCell(row, col);         // oma solu
  toggleLightsOutCell(row - 1, col);       // yläpuoli
  toggleLightsOutCell(row + 1, col);       // alapuoli
  toggleLightsOutCell(row, col - 1);       // vasen
  toggleLightsOutCell(row, col + 1);       // oikea
}

// Tarkistaa, onko peli voitettu (kaikki solut pois päältä)
bool checkLightsOutWin() {
  for (int r = 0; r < NUM_ROWS; r++){
    for (int c = 0; c < NUM_COLS; c++){
      if(lightsOutState[r][c]) return false;
    }
  }
  return true;
}

// Päivittää LED-matriisin Lights Out -pelin tilan mukaisesti
void updateLightsOutDisplay() {
  uint32_t matrix[9] = {0};
  for (int r = 0; r < NUM_ROWS; r++){
    for (int c = 0; c < NUM_COLS; c++){
      int index = r * NUM_COLS + c;
      if(lightsOutState[r][c]) {
        matrix[index] = strip.Color(255, 255, 255); // Valkoinen, kun solu päällä
      } else {
        matrix[index] = 0;
      }
    }
  }
  displayMatrix(matrix);
}

// Lights Out -pelin pääsilmukka. Käytetään järjestelmän readButtons()-funktiota, jonka palauttamien painallusten
// perusteella päivitetään pelin tilaa.
void runLightsOutGame() {
  initLightsOutGame();
  // Alustetaan oma tallennus painallusten tilalle
  int lo_prev[9] = {0};

  while (true) {
    readButtons(); // Päivitetään inputs[]
    
    // Tarkistetaan jokaisen napin osalta, onko painallus tapahtunut (siirtymä)
    for (int i = 0; i < 9; i++){
      if (inputs[i] > lo_prev[i]) { // Painallus havaittu
        int row = i / 3;         // Mappaus: indeksistä 0-8 -> 3x3-matriisi (rivi = i/3)
        int col = i % 3;         // sarake = i % 3
        toggleLightsOut(row, col);
      }
      lo_prev[i] = inputs[i];
    }
    
    updateLightsOutDisplay();
    
    // Jos peli voitettu, näytetään voittoefekti
    if (checkLightsOutWin()){
      for (int i = 0; i < 5; i++){
        uint32_t matrix[9];
        for (int j = 0; j < 9; j++){
          matrix[j] = strip.Color(0, 255, 0); // Vihreä
        }
        displayMatrix(matrix);
        delay(300);
        uint32_t clear[9] = {0};
        displayMatrix(clear);
        delay(300);
      }
      break;
    }
    delay(100);
  }
  inMenu = 1; // Palataan valikkoon pelin jälkeen
}


// ---------- Loput järjestelmäkoodista ----------


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
  return 0;
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
  inputs[0] = digitalRead(button1);
  inputs[3] = digitalRead(button2);
  inputs[6] = digitalRead(button3);

  //read B column
  selectButtons(2);
  inputs[1] = digitalRead(button1);
  inputs[4] = digitalRead(button2);
  inputs[7] = digitalRead(button3);

  //read C column
  selectButtons(3);
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
      changes++;                       
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
      for (int j = 0; j < 9; j++) {
        array[j] = 0;
      }
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
      for (int j = 0; j < 9; j++) {
        array[j] = 0;
      }
      if (prev) {
        array[i]++;
      }
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
            lcd.print("Lights Out      ");
            Game = 3;
            break;
          
          case 3:
            lcd.setCursor(3, 2);
            lcd.print("Simon says      ");
            Game = 4;
            break;
          case 4:
            lcd.setCursor(3, 2);
            lcd.print("Credits    ");
            Game = 5;
            break;
          case 5:
            lcd.setCursor(3, 2);
            lcd.print("Settings    ");
            Game = 6;
            break;
          
          default:
            lcd.setCursor(3, 2);
            lcd.print("Coming Soon...    ");
            Game = 7;
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
        tone(speaker, 330, 250);
        delay(250);
        tone(speaker, 392, 250);
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
  while ((millis() - start) < duration) {
    int rythmInputs[9] = { 0 };
    readButtons();
    singleInput(rythmInputs);
    for (int i = 0; i < 9; i++){
      if (rythmInputs[i]){
        if (i == (button - 1)){
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
        int noteDuration = 5000 / level->noteDurations[thisNote];
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

//Tic Tac Toe function
int* play(int play_board[], int kone, uint32_t colors[]) {
  int cur_player = player(play_board);
  uint32_t XColor = strip.Color(0, 0, 255);  // Pelaaja X on sininen
  uint32_t OColor = strip.Color(255, 0, 0);  // Pelaaja O on punainen
  //Kone tekee siirto
  if (kone && cur_player == X) {
    lcd.setCursor(3, 2);
    lcd.print("X's turn(machine)");
    delay(250);
    int move = bestMove(play_board);
    play_board[move] = X;
    tone(speaker, 262, 250);
    colors[move] = XColor;
    lcd.clear();
  } 
  //Ihminen tekee siirto
  else if ((!kone && cur_player == X) || cur_player == O) {
    int rythmInputs[9] = { 0 };
    readButtons();
    singleInput(rythmInputs);
    lcd.setCursor(3, 2);
    if (cur_player == X)
      lcd.print("X's turn");
    else if (cur_player == O)
      lcd.print("O's turn");
    for (int i = 0; i < 9; i++) {
      if (rythmInputs[i] && !play_board[i]) {
        tone(speaker, 262, 250);
        if (cur_player == X) {
          colors[i] = XColor;
          play_board[i] = X;
        } else if (cur_player == O) {
          colors[i] = OColor;
          play_board[i] = O;
        }
        lcd.clear();
        break;
      } 
      //Siirron tarkistus
      else if (rythmInputs[i] && play_board[i]) {
        tone(speaker, 200, 250);
        lcd.setCursor(3, 3);
        lcd.print("Invalid move");
        delay(750);
        break;
      }
    }
  }
  //Näyttää pelin tilanne
  displayMatrix(colors);
  return play_board;
}
void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(BRIGHTNESS);

  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(3, 1);
  lcd.print("Booting UP");
  colorWipe(strip.Color(255, 0, 0), 20);
  colorWipe(strip.Color(255, 255, 0), 20);
  colorWipe(strip.Color(0, 255, 0), 20);
  colorWipe(strip.Color(0, 255, 255), 20);
  colorWipe(strip.Color(255, 255, 255), 20);

  tone(speaker, 262, 250);
  delay(250);
  lcd.print(".");
  tone(speaker, 330, 250);
  delay(250);
  lcd.print(".");
  tone(speaker, 392, 250);
  delay(250);
  lcd.print(".");
  delay(250);

  lcd.clear();
  lcd.setCursor(3, 1);
  lcd.print("Select Game: ");
  colorWipe(strip.Color(0, 255, 0), 20);

  // Button config
  pinMode(buttonA, OUTPUT);
  pinMode(buttonB, OUTPUT);
  pinMode(buttonC, OUTPUT);

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
}



void showSimonSequence() {
  for (int i = 0; i < currentLength; i++) {
    int index = simonSequence[i];
    strip.setPixelColor(index, buttonColors[index]);
    strip.show();
    tone(speaker, 300 + index * 20, 300);
    delay(400);
    strip.setPixelColor(index, 0);
    strip.show();
    delay(200);
  }
}

bool checkSimonInput() {
  readButtons();
  for (int i = 0; i < 9; i++) {
    if (inputs[i] && !prev_inputs[i]) {
      strip.setPixelColor(i, buttonColors[i]);
      strip.show();
      tone(speaker, 300 + i * 20, 200);
      delay(300);
      strip.setPixelColor(i, 0);
      strip.show();
      delay(100);

      if (i != simonSequence[playerIndex]) {
        lcd.setCursor(3, 2);
        lcd.print("Game Over!     ");
        tone(speaker, 200, 1000);
        delay(2000);
        currentLength = 0;
        playerIndex = 0;
        return false;
      }

      playerIndex++;
      lastInputTime = millis();

      if (playerIndex >= currentLength) {
        delay(500);
        playerIndex = 0;
        simonSequence[currentLength++] = random(0, 9);
        showSimonSequence();
      }
    }
    prev_inputs[i] = inputs[i];
  }

  if (millis() - lastInputTime > inputTimeout) {
    lcd.setCursor(3, 2);
    lcd.print("Timeout!       ");
    tone(speaker, 150, 1000);
    delay(2000);
    currentLength = 0;
    playerIndex = 0;
    return false;
  }

  return true;
}


void loop() {
  // Valikko-tila
  if (inMenu) {
    readButtons();
    selectMenu(menu_Select);
    return;
  }

  // Pelivalinta
  switch (Game) {
    case 0:
      inMenu = 1;
      break;

    // Tic Tac Toe
    case 1:
      {
        int play_board[9] = { 0 };
        int kone = 0;
        int rythmInputs[9] = { 0 };
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("vs Human/Machine");
        lcd.setCursor(3, 2);
        lcd.print("0:Human 1:Machine");
        readButtons();
        singleInput(rythmInputs);
        //valitse ihmistä tai konetta vastaan
        while (!rythmInputs[0] && !rythmInputs[1]) {
          readButtons();
          singleInput(rythmInputs);
          if (rythmInputs[0]){
            tone(speaker, 330, 250);
            break;
          }
          else if (rythmInputs[1]) {
            kone = 1;
            tone(speaker, 330, 250);
            break;
          }
          for (int i = 2; i < 9; i++) {
            if (rythmInputs[i]) {
              tone(speaker, 200, 250);
              lcd.setCursor(3, 3);
              lcd.print("Invalid input");
              break;
            }
          }
        }
        lcd.clear();
        //pelaa kunnes on voittaja tai tasapeli
        while (!terminal(play(play_board, kone, colors)))
          continue;
        lcd.clear();
        //Näytölle ilmestyy voittajan nimi tai ilmoitus tasapelistä
        int winner = evaluate(play_board);
        if (winner == 1) {
          lcd.setCursor(3, 2);
          lcd.print("X win");
        } else if (winner == -1) {
          lcd.setCursor(3, 2);
          lcd.print("O win");
        } else {
          lcd.setCursor(3, 2);
          lcd.print("Stalemate");
        }
        delay(2000);
        lcd.clear();
        inMenu = 1;
        //Peli nollaa tilanteen ja palaa takaisin päävalikkoon
        for (int i = 0; i < 9; i++) {
          colors[i] = strip.Color(0, 0, 0);
        }
        break;
      }
    // Rythm Game
    case 2:
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

    // Lights Out (integroitu case 3:een)
    case 3:
      runLightsOutGame();
      break;

    case 4:
        if (!playingSimon) {
        lcd.setCursor(3, 2);
        lcd.print("Simon says!      ");
        delay(1000);
        displayMatrix(colors);
        randomSeed(analogRead(A0));
        currentLength = 1;
        simonSequence[0] = random(0, 9);
        playerIndex = 0;
        showSimonSequence();
        lastInputTime = millis();
        playingSimon = true;
      } else {
        if (!checkSimonInput()) {
          playingSimon = false;
          delay(1000);
          inMenu = 1;
        }
      }
        break;

    case 5:
      
      lcd.clear();
      lcd.setCursor (0,0);
      lcd.print("Tic-Tac-Toe: Minh ");
      lcd.setCursor (0,1);
      lcd.print("Rythm Game: Mael ");
      lcd.setCursor (0,2);
      lcd.print("Lights Out: Rauno ");
      lcd.setCursor (0,3);
      lcd.print("Simon Says: Saimi ");
      delay(8000);
      lcd.clear();
      inMenu = 1;
      break;

    case 6:
      lcd.clear();
      inMenu = 1;
      break;
    case 7:
       lcd.clear();
      inMenu = 1;
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

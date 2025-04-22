
#include <LiquidCrystal_I2C.h>
#include <FastLED_NeoPixel.h>

#define ALL 4
#define DATA_PIN 2
#define NUM_LEDS 9
#define BRIGHTNESS 100

int valiaika = 1000; 
int increase = 200;
int correct_color = 0;
int ronda = 0;

int points_player1 = 0;
int points_player2 = 0;
int round2 = 0;  // usaremos esto solo como contador de rondas multijugador
int target_player2 = 0;

// Botones
const int buttonA = 8;
const int buttonB = 9;
const int buttonC = 10;

const int button1 = 11;
const int button2 = 12;
const int button3 = 13;

// Speaker
const int speaker = 3;

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Matrices
uint32_t colors[9] = { 0 };
int inputs[9] = { 0 };
int prev_inputs[9] = { 0 };

// FastLED
FastLED_NeoPixel<NUM_LEDS, DATA_PIN, NEO_GRB> strip;

// Menú
int inMenu = 1;
int menu_Select[9] = { 0 };
int Game = 0;

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(BRIGHTNESS);

  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(3, 1);
  lcd.print("Booting UP");

  performColorWipe(strip.Color(255, 0, 0), 20);
  performColorWipe(strip.Color(255, 255, 0), 20);
  performColorWipe(strip.Color(0, 255, 0), 20);
  performColorWipe(strip.Color(0, 255, 255), 20);
  performColorWipe(strip.Color(255, 255, 255), 20);

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
  performColorWipe(strip.Color(0, 255, 0), 20);

  // Configuración botones
  pinMode(buttonA, OUTPUT);
  pinMode(buttonB, OUTPUT);
  pinMode(buttonC, OUTPUT);

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
}

void digitalTripleWrite(int pin1, int pin2, int pin3, int select, int value) {
  digitalWrite(pin1, ((select == 1 || select == ALL) ? value : !value));
  digitalWrite(pin2, ((select == 2 || select == ALL) ? value : !value));
  digitalWrite(pin3, ((select == 3 || select == ALL) ? value : !value));
}

int displayMatrix(uint32_t color[9]) {
  for (int i = 0; i < 9; i++) {
    strip.setPixelColor(i, color[i]);
  }
  strip.show();
}

void selectButtons(int column) {
  digitalTripleWrite(buttonA, buttonB, buttonC, column, HIGH);
}

int readButtons() {
  for (int i = 0; i < 9; i++) {
    inputs[i] = 0;
  }

  selectButtons(1);
  inputs[0] = digitalRead(button1);
  inputs[3] = digitalRead(button2);
  inputs[6] = digitalRead(button3);

  selectButtons(2);
  inputs[1] = digitalRead(button1);
  inputs[4] = digitalRead(button2);
  inputs[7] = digitalRead(button3);

  selectButtons(3);
  inputs[2] = digitalRead(button1);
  inputs[5] = digitalRead(button2);
  inputs[8] = digitalRead(button3);

  selectButtons(0);
  return inputs;
}

int toggleArray(uint32_t array[9], uint32_t value) {
  int changes = 0;
  for (int i = 0; i < 9; i++) {
    if (inputs[i] > prev_inputs[i]) {
      array[i] = (!array[i] * value);
      changes++;
    }
    prev_inputs[i] = inputs[i];
  }
  return changes;
}

void selectMenu(int array[9]) {
  int set = 0;
  for (int i = 0; i < 9; i++) {
    int prev = array[i];
    if (inputs[i] > prev_inputs[i]) {
      for (int j = 0; j < 9; j++) array[j] = 0;
      if (prev) array[i]++;
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
          case 0: lcd.setCursor(3, 2); lcd.print("Tic Tac Toe       "); Game = 1; break;
          case 1: lcd.setCursor(3, 2); lcd.print("Rythm Game       "); Game = 2; break;
          case 2: lcd.setCursor(3, 2); lcd.print("Simon Says      "); Game = 3; break;
          case 3: lcd.setCursor(3, 2); lcd.print("Speed Game      "); Game = 4; break;
          case 4: lcd.setCursor(3, 2); lcd.print("Multiplayer Game"); Game = 5; break;
          case 5: lcd.setCursor(3, 2); lcd.print("Settings         "); break;
          case 6: lcd.setCursor(3, 2); lcd.print("Credits          "); break;
          default: lcd.setCursor(3, 2); lcd.print("Coming Soon...   "); break;
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

void pattern() {
  uint32_t colors8 = strip.Color(255, 0, 0);
  for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, colors8);
  correct_color = random(0, NUM_LEDS);
  strip.setPixelColor(correct_color, strip.Color(0, 255, 0));
  strip.show();

}

void pattern2() {
  uint32_t verde = strip.Color(255, 0, 0);
  uint32_t Red = strip.Color(0, 255, 0);
  uint32_t azul = strip.Color(0, 0, 255);

  int player1 = random(0, NUM_LEDS);
  int player2 = random(0, NUM_LEDS);
  while (player1 == player2) player2 = random(0, NUM_LEDS);

  for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, verde);
  strip.setPixelColor(player1, Red);
  strip.setPixelColor(player2, azul);
  strip.show();

  correct_color = player1;
  target_player2 = player2; // nueva variable
}

void loop() {
  static int score = 0;  // solo para modo 1 jugador

  if (inMenu) {
    readButtons();
    selectMenu(menu_Select);
    return;
  }

  switch (Game) {
    case 0:
      inMenu = 1;
      break;

    case 1:
      readButtons();
      toggleArray(colors, strip.Color(0, 0, 255));
      displayMatrix(colors);
      break;

    case 2:
      readButtons();
      toggleArray(colors, strip.Color(255, 255, 0));
      displayMatrix(colors);
      break;

    case 3:
      // Simon Says por implementar
      break;

    case 4: {
      ronda++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Round: ");
      lcd.print(ronda);

      pattern();

      int score = 0;
      int correctButton = correct_color;

      bool Gamerunning = true;
      

      while (Gamerunning) {
        readButtons();
        bool botonpresionado = false; 

        for (int i = 0; i < 9; i++) {
          if (inputs[i] == LOW) {
            if (i == correctButton) {
              tone(speaker, 523, 500);
              score++;
              correctButton = random(0,NUM_LEDS);
              pattern();
              botonpresionado = true;
              delay(5000);
            } else {
              tone(speaker, 440, 500);
              Gamerunning = false;
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Score:");
              lcd.print(score);
              delay(3000);
              lcd.clear();
              lcd.setCursor(1, 1);
              lcd.print("Game Over!");
              delay(1000);
              lcd.clear();
              score = 0;
              inMenu = 1;
              return;
            }
          }
        }
      }
      valiaika = max(increase, valiaika - increase);
      delay(valiaika);
      break;
    }

    case 5: {
      round2 = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Player 1(Red): ");
      lcd.print(points_player1);
      lcd.setCursor(0, 1);
      lcd.print("Player 2(Blue): ");
      lcd.print(points_player2);

      pattern2();

      while (round2 < 12) {
        bool botonpresionado = false;
        while (!botonpresionado) {
          readButtons();
          for (int i = 0; i < 9; i++) {
            if (inputs[i] == LOW) {
              if (i == correct_color) {
                tone(speaker, 523, 500);
                points_player1++;
                botonpresionado = true;
                delay(1000);
                break;
              }
              if (i == target_player2) {
                tone(speaker, 440, 500);
                points_player2++;
                botonpresionado = true;
                delay(1000);
                break;
              }
            }
          }
        }
        round2++;
        pattern2();
      }

      lcd.clear();
      if (points_player1 > points_player2) {
        lcd.setCursor(1, 0);
        lcd.print("Player 1 Wins!");
        delay(2000);
        lcd.clear();
      }
      else if (points_player1 < points_player2) {
        lcd.setCursor(1, 0);
        lcd.print("Player 2 Wins!");
        delay(2000);
        lcd.clear();
      }
      else {
        lcd.setCursor(1, 0);
        lcd.print("Tie!");
        delay(2000);
        lcd.clear();
      }
      delay(1000);
      points_player1 = 0;
      points_player2 = 0;
      inMenu = 1;
      break;
    }
  }
}

void performColorWipe(uint32_t color, unsigned long wait) {
  for (unsigned int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

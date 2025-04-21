#include <LiquidCrystal_I2C.h>
#include <FastLED_NeoPixel.h>
#define ALL 4
#define DATA_PIN 2
#define NUM_LEDS 9
#define BRIGHTNESS 100

// Buttons
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

// FastLED Setup
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

// Funciones
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
  for (int i = 0; i < 9; i++) inputs[i] = 0;
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
          case 3: lcd.setCursor(3, 2); lcd.print("Settings      "); break;
          case 4: lcd.setCursor(3, 2); lcd.print("Credits    "); break;
          default: lcd.setCursor(3, 2); lcd.print("Coming Soon...    "); break;
        }
        break;
      case 2:
        array[i] = 1;
        lcd.setCursor(3, 1); lcd.print("             ");
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
      if (!playingSimon) {
        lcd.setCursor(3, 2);
        lcd.print("Simon says!      ");
        delay(1000);
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
  }
}

void colorWipe(uint32_t color, unsigned long wait) {
  for (unsigned int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

#include <LiquidCrystal_I2C.h>
#include <FastLED_NeoPixel.h>
#define ALL 4
#define DATA_PIN 2
#define NUM_LEDS 9
#define BRIGHTNESS 100

// botones
const int buttonA = 8;
const int buttonB = 9;
const int buttonC = 10;

const int button1 = 11;
const int button2 = 12;
const int button3 = 13;

// Parlante
const int speaker = 3;

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Matrices
uint32_t colors[9] = { 0 };  // Esta es la matriz de colores que displayMatrix usará.
int inputs[9] = { 0 };
int prev_inputs[9] = { 0 };

// Fast Led Setup y Colores
FastLED_NeoPixel<NUM_LEDS, DATA_PIN, NEO_GRB> strip;

// Menú
int inMenu = 1;
int menu_Select[9] = { 0 };
int Game = 0;

// Variables para el juego Simon Dice
int sequence[100];  // Arreglo que almacena la secuencia de luces
int seqLength = 0;   // Longitud de la secuencia actual
int playerInput[100];  // Entrada del jugador
int playerPos = 0;    // Posición actual en la secuencia del jugador

void setup() {
  Serial.begin(9600);
  strip.begin();  // Inicializa el strip (¡necesario!)
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

  pinMode(buttonA, OUTPUT);
  pinMode(buttonB, OUTPUT);
  pinMode(buttonC, OUTPUT);

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
}

// Funciones para controlar los LEDs

void digitalTripleWrite(int pin1, int pin2, int pin3, int select, int value) {
  digitalWrite(pin1, ((select == 1 || select == ALL) ? value : !value));
  digitalWrite(pin2, ((select == 2 || select == ALL) ? value : !value));
  digitalWrite(pin3, ((select == 3 || select == ALL) ? value : !value));
}

// Toma los LEDs como entrada y los muestra en la matriz de LEDs
int displayMatrix(uint32_t color[9]) {
  for (int i = 0; i < 9; i++) {
    strip.setPixelColor(i, color[i]);
  }
  strip.show();
}

// Selecciona una columna en la matriz de botones y la enciende
void selectButtons(int column) {
  digitalTripleWrite(buttonA, buttonB, buttonC, column, HIGH);
}

// Lee los botones de la matriz de botones
int readButtons() {
  for (int i = 0; i < 9; i++) {
    inputs[i] = 0;
  }

  // Lee la columna A
  selectButtons(1);
  inputs[0] = digitalRead(button1);
  inputs[3] = digitalRead(button2);
  inputs[6] = digitalRead(button3);

  // Lee la columna B
  selectButtons(2);
  inputs[1] = digitalRead(button1);
  inputs[4] = digitalRead(button2);
  inputs[7] = digitalRead(button3);

  // Lee la columna C
  selectButtons(3);
  inputs[2] = digitalRead(button1);
  inputs[5] = digitalRead(button2);
  inputs[8] = digitalRead(button3);

  selectButtons(0);
  return inputs;
}

// Función que enciende y apaga los LEDs según la entrada
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

// Función que maneja la selección del menú
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

// Lógica para Simon Dice
void startSimonSays() {
  seqLength = 1;  // Comienza con una secuencia de longitud 1
  generateSequence();  // Genera la secuencia inicial
  playerPos = 0;  // Reinicia la posición del jugador
  showSequence();  // Muestra la secuencia al jugador
}

void generateSequence() {
  // Genera una nueva secuencia aleatoria de LEDs
  sequence[seqLength - 1] = random(0, NUM_LEDS);
}

void showSequence() {
  // Muestra la secuencia de LEDs en la tira
  for (int i = 0; i < seqLength; i++) {
    strip.setPixelColor(sequence[i], strip.Color(255, 0, 0));  // Rojo para indicar la secuencia
  }
  strip.show();
  delay(1000);  // Espera 1 segundo
  for (int i = 0; i < seqLength; i++) {
    strip.setPixelColor(sequence[i], strip.Color(0, 0, 0));  // Apaga el LED
  }
  strip.show();
  delay(500);  // Pausa entre las secuencias
}

bool checkPlayerInput() {
  // Verifica si la entrada del jugador es correcta
  for (int i = 0; i <= playerPos; i++) {
    if (playerInput[i] != sequence[i]) {
      return false;  // Si alguna entrada no coincide, el jugador perdió
    }
  }
  return true;  // El jugador ha acertado hasta ahora
}

void resetGame() {
  // Resetea el juego
  seqLength = 1;
  playerPos = 0;
  for (int i = 0; i < 100; i++) {
    sequence[i] = 0;
    playerInput[i] = 0;
  }
}

void loop() {
  if (inMenu) {
    readButtons();
    selectMenu(menu_Select);
    return;
  }

  switch (Game) {
    case 0:  // Sin juego, volver al menú
      inMenu = 1;
      break;

    case 1:  // Tic Tac Toe
      // Lógica del juego de Tic Tac Toe
      break;

    case 2:  // Rythm Game
      // Lógica del juego de Ritmo
      break;

    case 3:  // Simon Says
      if (seqLength == 1 && playerPos == 0) {
        startSimonSays();  // Comienza el juego Simon Dice
      }

      readButtons();  // Lee los botones del jugador
      if (inputs[0] > prev_inputs[0]) {  // Si el jugador presiona el botón 1 (por ejemplo)
        playerInput[playerPos] = 0;
        playerPos++;
        if (!checkPlayerInput()) {
          lcd.clear();
          lcd.setCursor(3, 1);
          lcd.print("Game Over!");
          tone(speaker, 440, 500);  // Tono de error
          delay(1000);
          resetGame();  // Resetea el juego después de un error
        }
      }

      // Repetir la secuencia si el jugador la ha completado correctamente
      if (playerPos == seqLength) {
        seqLength++;  // Aumenta la longitud de la secuencia
        playerPos = 0;  // Reinicia la posición del jugador
        generateSequence();
        showSequence();
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

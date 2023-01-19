// Author: blali733
// Version 1.2.0
// Updated 18.01.2022
// Required libraries:
// * hd44780 by Bill Perry

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

// Digital pins
#define KEY_LEFT 4
#define KEY_RIGHT 7
#define KEY_UP 6
#define KEY_DOWN 5
#define KEY_CENTER 8
// Analog pins
#define MIC 0
#define POTENTIOMETER 1
#define TEMP 2
#define BRIGHTNESS 3

// display
hd44780_I2Cexp lcd(0x20, I2Cexp_MCP23008, 7, 6, 5, 4, 3, 2, 1, HIGH);

// constants
char spinner[] = { "|/-\001|/-\001" };
#define SPINNER_ITEMS 8
#define DISPLAY_H 2
#define DISPLAY_W 16
#define SPINNER_DELAY 150
#define DEBOUNCE_DELAY 250
#define SCAN_DELAY 50
#define DEFAULT_CLASS_SIZE 25
#define SPLASH_DELAY 1000

uint8_t backslash[8]  = {0x10,0x08,0x04,0x02,0x01,0x00,0x00};

int classSize = DEFAULT_CLASS_SIZE;
int spinnerIterator = 0;

void setup() {
  // initialize utils:
  lcd.begin(DISPLAY_W, DISPLAY_H);

  // initialize pin modes:
  pinMode(KEY_LEFT, INPUT);
  pinMode(KEY_RIGHT, INPUT);
  pinMode(KEY_UP, INPUT);
  pinMode(KEY_DOWN, INPUT);
  pinMode(KEY_CENTER, INPUT);
  pinMode(MIC, INPUT);
  pinMode(POTENTIOMETER, INPUT);
  pinMode(TEMP, INPUT);
  pinMode(BRIGHTNESS, INPUT);

  int mic = analogRead(MIC);
  int pot = analogRead(POTENTIOMETER);
  int temp = analogRead(TEMP);
  int bri = analogRead(BRIGHTNESS);
  lcd.createChar(1, backslash);

  long seed = (mic+pot+bri)*(long)temp;
  randomSeed(seed);
  printSeed(seed);
  delay(SPLASH_DELAY);
}

void loop() {
  bool setup = true;
  printConfig();
  while (setup == true) {
    if (digitalRead(KEY_UP)) {
      classSize++;
      printNumber(classSize);
      delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(KEY_DOWN)) {
      classSize--;
      printNumber(classSize);
      delay(DEBOUNCE_DELAY);
    }
    if (digitalRead(KEY_CENTER)) {
      setup = false;
      delay(DEBOUNCE_DELAY);
    }

    delay(SCAN_DELAY);
  }

  printIdle();

  while (1) {
    if (digitalRead(KEY_CENTER)) {
      printRandomizing();

      int result = random(0, classSize) + 1;

      lcd.setCursor(8, 1);

      do {
        spinnerIterator = spinnerIterator % SPINNER_ITEMS;
        lcd.print(spinner[spinnerIterator]);
        lcd.setCursor(8, 1);
        spinnerIterator++;
        delay(SPINNER_DELAY);
      } while (digitalRead(KEY_CENTER));

      printResult(result);
    }

    delay(SCAN_DELAY);
  }
}

void printConfig() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ilosc uczniow");
  printNumber(classSize);
}

void printRandomizing() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Losowanie");
}

void printIdle() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Oczekiwanie");
  lcd.setCursor(0, 1);
  lcd.print("Losuj - srodek");
}

void printResult(int result) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wylosowany numer");
  printNumber(result);
}

void printSeed(long seed) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Random seed");
  printNumber(seed);
}

void printNumber(int number) {
  lcd.setCursor(0, 1);
  lcd.print(number);
}

void printNumber(long number) {
  lcd.setCursor(0, 1);
  lcd.print(number);
}

// notes
//0123456789abcdef
//Ilosc uczniow
//Wylosowany numer
//Losowanie
//Oczekiwanie
//Losuj - srodek
//Random seed

// Author: blali733
// Version 1.1.1
// Updated 18.01.2022
// Required libraries:
// * hd44780 by Bill Perry

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

// buttons
#define KEY_LEFT 4
#define KEY_RIGHT 7
#define KEY_UP 6
#define KEY_DOWN 5
#define KEY_CENTER 8
// mic
#define MIC 0

// display
hd44780_I2Cexp lcd(0x20, I2Cexp_MCP23008, 7, 6, 5, 4, 3, 2, 1, HIGH);

// constants
char spinner[] = { "|/-\\|/-\\" };
#define SPINNER_ITEMS 8
#define DISPLAY_H 2
#define DISPLAY_W 16
#define SPINNER_DELAY 100
#define DEBOUNCE_DELAY 250
#define SCAN_DELAY 50
#define DEFAULT_CLASS_SIZE 25

int classSize = DEFAULT_CLASS_SIZE;
int spinnerIterator = 0;

void setup() {
  // initialize utils:
  lcd.begin(DISPLAY_W, DISPLAY_H);
  randomSeed(analogRead(MIC));

  // initialize pin modes:
  pinMode(KEY_LEFT, INPUT);
  pinMode(KEY_RIGHT, INPUT);
  pinMode(KEY_UP, INPUT);
  pinMode(KEY_DOWN, INPUT);
  pinMode(KEY_CENTER, INPUT);
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

void printNumber(int number){
  char resultArr[5];
  itoa(number, resultArr, 10);

  lcd.setCursor(0, 1);
  lcd.print(resultArr);
}

// notes
//0123456789abcdef
//Ilosc uczniow
//Wylosowany numer
//Losowanie
//Oczekiwanie
//Losuj - srodek

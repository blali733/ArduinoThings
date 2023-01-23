#include <IRremote.hpp>

#define IR_PIN 3
#define LED_FEEDBACK_PIN 13
#define BAUD_RATE 115200

void setup() {
  Serial.begin(BAUD_RATE);
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK, LED_FEEDBACK_PIN);
}

void loop() {
  if (IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.resume();
  }
  delay(100);
}
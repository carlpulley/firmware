#include "application.h"

/**
 * Simple application that monitors a pin and prints out that pins digital and analog value.
 **/

 // Vary this experimental value in order to tune the tap sensitivity
#define LOOP_WAIT 90 // milliseconds
// Pin that we should observe
#define PIN A0

void setup() {
  pinMode(PIN, INPUT);
}

void loop() {
  String tap;
  if (digitalRead(PIN) == 1) {
    tap = "yes";
  } else {
    tap = "no ";
  }
  Serial.println(tap+" ["+String(analogRead(PIN))+"]");

  delay(LOOP_WAIT);
}

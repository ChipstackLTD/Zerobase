// Sq example

#include <Adafruit_TinyUSB.h>

// Syntax:
// random(max)
// random(min, max)
// Returns: A random number between min and max-1. Data type: long.

long randNumber;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // print a random number from 0 to 299
  randNumber = random(300);
  Serial.print("Randum number from 0 to 299: ");
  Serial.println(randNumber);

  // print a random number from 10 to 19
  randNumber = random(10, 20);
  Serial.print("Randum number from 10 to 20: ");
  Serial.println(randNumber);

  Serial.println();
  delay(1000);
}
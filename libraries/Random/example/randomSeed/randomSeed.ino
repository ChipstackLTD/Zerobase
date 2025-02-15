// randomSeed example

#include <Adafruit_TinyUSB.h>

// Syntax: randomSeed(seed)
// Returns: Nothing.

int seed = 1234;
long randNumber;

void setup() {
  Serial.begin(9600);
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
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
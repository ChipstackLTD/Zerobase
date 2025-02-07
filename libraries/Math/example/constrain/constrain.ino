// Constrain example

#include <Adafruit_TinyUSB.h>

// Syntax: constrain(x, a, b)
// Returns:
// x: if x is between a and b
// a: if x is less than a
// b: if x is greater than b

int x = 200;
int a = 40;
int b = 180;
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  x = constrain(x, a, b);
  Serial.print("Constrain value: ");
  Serial.println(x);
}

void loop() {
}

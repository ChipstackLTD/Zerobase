// Pow example

#include <Adafruit_TinyUSB.h>

// Syntax: pow(base, exponent)
// Returns: The result of the exponentiation. Data type: double.

int base = 2;
int exponent = 3;
int res = 0;
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  res = pow(base, exponent);
  Serial.print("Pow value: ");
  Serial.println(res);
}

void loop() {
}

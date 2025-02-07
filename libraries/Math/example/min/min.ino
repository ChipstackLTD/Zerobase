// Min example

#include <Adafruit_TinyUSB.h>

// Syntax: min(x, y)
// Returns: The smaller of the two numbers.

int x = 200;
int y = 300;
int res = 0;
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  res = min(x, y);
  Serial.print("Min value: ");
  Serial.println(res);
}

void loop() {
}

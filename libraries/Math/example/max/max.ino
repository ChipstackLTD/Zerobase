// Max example

#include <Adafruit_TinyUSB.h>

// Syntax: max(x, y)
// Returns: The larger of the two parameter values.

int x = 200;
int y = 300;
int res = 0;
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  res = max(x, y);
  Serial.print("Max value: ");
  Serial.println(res);
}

void loop() {
}

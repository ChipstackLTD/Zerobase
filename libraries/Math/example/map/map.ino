// map example

#include <Adafruit_TinyUSB.h>

// Syntax: map(value, fromLow, fromHigh, toLow, toHigh)
// Returns: The mapped value. Data type: long.

int val = 1023;
int fromLow = 0;
int fromHigh = 1023;
int toLow = 0;
int toHigh = 255;
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  val = map(val, fromLow, fromHigh, toLow, toHigh);
  Serial.print("Map value: ");
  Serial.println(val);
}

void loop() {
}

// Sq example

#include <Adafruit_TinyUSB.h>

// Syntax: sq(x)
// Returns: The square of the number. Data type: double.

int x = 0;
int res = 0;
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  // Because of the way the sq() function is implemented, avoid using other functions inside the brackets, it may lead to incorrect results.
  // int x = sq(Serial.parseInt()); // avoid this
}

void loop() {
  x = Serial.parseInt();  // keep other operations outside the sq function
  if (x != 0) {
    int res = sq(x);
    Serial.print("x value: ");
    Serial.println(x);
    Serial.print("Square value: ");
    Serial.println(res);
  }
}

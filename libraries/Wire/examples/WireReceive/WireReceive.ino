// I2C receive data example
// Schematic:
// *** NOTE: If you use Zerobase 2 board, you need a logic shifter to shift from 3.3V (Zerobase 2) to 5V (Arduino Board) *** //
// Zerobase board don't need logic shifter
// If you use I2C1 (D3 - SDA, D2 - SCL) --> Using Wire1.begin(); (Available on Zerobase 2)
// If you use I2C2 (D18 - SDA, D19 - SCL) --> Using Wire.begin();

//Zerobase 2
// 3.3V --> LV
// GND --> GND
// SDA --> LV1
// SCL --> LV2

// Arduino Board
// 5V --> HV
// GND --> GND
// SDA --> HV1
// SCL --> HV2

#include <Adafruit_TinyUSB.h>
#include <Wire.h>

void setup() {
  Wire.begin(8); // Initialize as I2C slave with address 8
  Wire.onReceive(receiveEvent); // Register receive event
  Serial.begin(9600);
}

void loop() {
  // Do nothing, waiting for data from master
}

void receiveEvent(int howMany) {
  Serial.print("Received: ");
  while (Wire.available()) {
    char c = Wire.read(); // Read incoming byte
    Serial.print(c);
  }
  Serial.println();
}


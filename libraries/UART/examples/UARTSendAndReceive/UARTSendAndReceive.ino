// UART send and receive data example
// Schematic:
// *** NOTE: If you use Zerobase 2 board, you need a logic shifter to shift from 3.3V (Zerobase 2) to 5V (Arduino Board) *** //

// Zerobase board don't need logic shifter

//Zerobase 2
// 3.3V --> LV
// GND --> GND
// tx --> LV1
// rx --> LV2

// Arduino Board
// 5V --> HV
// GND --> GND
// rx --> HV1
// tx --> HV2


#include <Adafruit_TinyUSB.h>

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
}

void loop() {
  // Send data
  Serial2.println("This is Serial 2");
  
  // Receive data
  if (Serial2.available()) {
    String receivedData = Serial2.readStringUntil('\n');  // Reads until newline character
    Serial.println(receivedData);
  }
  delay(1000);
}

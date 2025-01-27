// Spi Send and Receive Example
// This example communicate between Zerobase 2 and Arduino Mega
// Schematic: you need a Logic Level Shifter to shift logic level 3.3V from Zerobase 2 to 5V of Arduino (in this Example using Arduino Mega)

// Zerobase 2
// ss --> lv1
// mo --> lv2
// 3.3V --> lv
// gnd --> gnd
// mi --> lv3
// sck --> lv4

// Arduino mega
// ss --> hv1
// mo --> hv2
// 5V --> hv
// gnd --> gnd
// mi --> hv3
// sck --> hv4

#include <Adafruit_TinyUSB.h>
#include <SPI.h>

#define SS 10  // Slave Select pin

void setup() {
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);  // Deselect slave
  SPI.begin();
  Serial.begin(9600);
}

void loop() {
  const char* sentString = "Hello World";  // String to send (11 characters)
  char receivedString[12] = { 0 };         // Buffer for received string (+1 for null terminator)

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(SS, LOW);   // Select slave
  delayMicroseconds(100);  // Allow slave to prepare

  // Send and receive each character
  for (int i = 0; i < 11; i++) {
    receivedString[i] = SPI.transfer(sentString[i]);
    delayMicroseconds(200);  // Allow slave to process
  }

  digitalWrite(SS, HIGH);  // Deselect slave
  SPI.endTransaction();

  receivedString[11] = '\0';  // Null-terminate the received string

  // Debugging
  Serial.print("String sent: ");
  Serial.println(sentString);

  Serial.print("String received: ");
  Serial.println(receivedString);
  Serial.println();

  delay(1000);
}

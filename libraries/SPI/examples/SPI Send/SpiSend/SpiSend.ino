// SPI SEND

#include <Adafruit_TinyUSB.h>

#include <SPI.h>  // Include SPI library

#define SS 10  // Slave Select pin

void setup() {
  // Set SS pin as OUTPUT and set it HIGH
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);

  // Initialize SPI communication
  SPI.begin();
  Serial.begin(9600);
}

void loop() {
  const char* message = "Hello World";  // The message to send
  uint8_t length = strlen(message);     // Get the length of the message

  // Start SPI transaction
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  digitalWrite(SS, LOW);  // Select the slave

  // Send the length of the message
  SPI.transfer(length);

  // Send each character of the message
  for (uint8_t i = 0; i < length; i++) {
    SPI.transfer(message[i]);  // Send one character
  }

  digitalWrite(SS, HIGH);  // Deselect the slave

  SPI.endTransaction();  // End SPI transaction

  Serial.println("Message sent: Hello World");

  delay(1000);  // Wait for 1 second before sending again
}

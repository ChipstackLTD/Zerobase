
// Ultrasonic Sensor with PulseIn
#include <Adafruit_TinyUSB.h>

#define trigPin 8
#define echoPin 7

int duration = 0;
float distance;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  Serial.println("Serial initialized with TinyUSB");
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Your loop code here
  // Write a pulse to the HC-SR04 Trigger Pin
  duration = 0;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // // Measure the response from the HC-SR04 Echo Pin
  // while (digitalRead(echoPin) == 0) {
  // }

  // while (digitalRead(echoPin) == 1) {
  //   duration++;
  // }

  duration = pulseIn(echoPin, HIGH);
  // Serial.println(duration);
  // // Determine distance from duration
  // // Use 343 metres per second as speed of sound
  Serial.print("Duration from pulseIn: ");
  Serial.println(duration);
  distance = ((duration / 2) * 0.0343);

  // Send results to Serial Monitor

  Serial.print("Distance = ");
  if (distance >= 400 || distance <= 2) {
    Serial.println("Out of range");
  } else {
    Serial.print(distance);  // Print distance with 2 decimal places
    Serial.println(" cm");
  }
  delay(500);
}

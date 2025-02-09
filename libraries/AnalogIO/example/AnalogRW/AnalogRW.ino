/*
  AnalogRead AnalogWrite Example
*/

int led = A2;
int potPin = A1;
void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  pinMode(potPin, INPUT_ANALOG);
}

void loop() {
  int sensorValue = analogRead(potPin);
  int outputValue = map(sensorValue, 0, 1023, 0, 255);
  analogWrite(led, outputValue);
}

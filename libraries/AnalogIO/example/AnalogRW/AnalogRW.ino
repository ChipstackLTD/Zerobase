/*
  AnalogRead AnalogWrite Example
*/

int led = A1;
int potPin = A2;
void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  pinMode(potPin, INPUT_ANALOG);
}

void loop() {
  int sensorValue = analogRead(potPin);
#ifdef BOARD_ZEROBASE
  sensorValue = map(sensorValue, 0, 1023, 0, 4095);
#endif
  analogWrite(led, sensorValue);
}

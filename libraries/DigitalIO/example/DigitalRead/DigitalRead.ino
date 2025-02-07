/*
  digitalWriteLed Example
*/

int btn = 14;
int led = 3;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(btn, INPUT_PULLUP);
  pinMode(led, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  if (digitalRead(btn)) {
    digitalWrite(led, LOW);
  } else {
    digitalWrite(led, HIGH);
  }
}

/*
  Interrupt Example
*/

int btn = 14;
int led = 3;

// Boolean to represent toggle state
volatile bool toggleState = false;
// Variable to store the last debounce time
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 100;  // 50 ms debounce delay

void checkSwitch() {
  // Record the current time
  unsigned long currentTime = millis();

  // Check if enough time has passed since the last interrupt
  if ((currentTime - lastDebounceTime) > debounceDelay) {
    // Check the button state and toggle the LED
    if (digitalRead(btn) == LOW) {
      toggleState = !toggleState;      // Toggle the state
      digitalWrite(led, toggleState);  // Set the LED state
    }

    // Update the last debounce time
    lastDebounceTime = currentTime;
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  // pinMode(btn, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  attachInterrupt(btn, GPIO_Mode_IPU, checkSwitch, EXTI_Mode_Interrupt, EXTI_Trigger_Falling);
}

// the loop function runs over and over again forever
void loop() {
  delay(5000);
}

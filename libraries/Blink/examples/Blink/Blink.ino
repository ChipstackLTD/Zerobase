/*
  Blink Example
*/

// Check if the board is Zerobase (add specific macro to Zerobase in boards.txt)
#ifdef BOARD_ZEROBASE
void zerobaseInit(){
  if (FLASH->STATR & (1<<14)) NVIC_SystemReset();
  SystemReset_StartMode(Start_Mode_BOOT);
  pinMode(PD4, OUTPUT);
}
#endif

// the setup function runs once when you press reset or power the board
void setup() {
  // Only call zerobaseInit() for Zerobase
  #ifdef BOARD_ZEROBASE
  zerobaseInit();
  #endif
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off (LOW is the voltage level)
  delay(1000);
}

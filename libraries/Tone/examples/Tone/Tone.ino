
int buzzer = 3;

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzer, OUTPUT);
  tone(buzzer, 2000);
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Frequency: 20Hz - 20Khz
  for (int frquency = 20; frquency <= 20000; frquency++) {
    tone(buzzer, frquency);
  }
}

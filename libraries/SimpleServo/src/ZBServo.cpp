// #include "SimpleServo.h"
// #include "Arduino.h"

// SimpleServo::SimpleServo(int pin) {
//   servoPin = pin;
//   currentAngle = 0;
//   lastPulseTime = 0;
// }

// void SimpleServo::attach() {
//   pinMode(servoPin, OUTPUT);
// }

// void SimpleServo::write(int angle) {
//   // Clamp angle to valid range
//   currentAngle = constrain(angle, 0, 180);
// }

// void SimpleServo::update() {
//   unsigned long currentTime = micros();
  
//   // Send pulses approximately every 20ms
//   if (currentTime - lastPulseTime >= 20000) { // 20ms in microseconds
//     int pulseWidth = map(currentAngle, 0, 180, 500, 2500);
    
//     digitalWrite(servoPin, HIGH);
//     delayMicroseconds(pulseWidth);
//     digitalWrite(servoPin, LOW);
    
//     lastPulseTime = currentTime;
//   }
// }


#include "ZBServo.h"
#include "Arduino.h"

ZBServo::ZBServo() {


}

void ZBServo::attach(int pin, int SERVO_MIN_PULSE, int SERVO_MAX_PULSE) {
  servoPin = pin;
  currentAngle = 0;
  this->SERVO_MAX_PULSE = SERVO_MAX_PULSE;
  this->SERVO_MIN_PULSE = SERVO_MIN_PULSE;
     // Simplified initialization
  PinName digToPin = digitalPinToPinName(servoPin);
  servoTimer.setup((TIM_TypeDef*)pinmap_peripheral(digToPin, PinMap_PWM));
  servoChannel = CH_PIN_CHANNEL(pinmap_function(digToPin, PinMap_PWM));

  servoTimer.setOverflow(20000, MICROSEC_FORMAT);
  servoTimer.setMode(servoChannel, TIMER_OUTPUT_COMPARE_PWM1, servoPin);
  servoTimer.setCaptureCompare(servoChannel, 1500, MICROSEC_COMPARE_FORMAT);
  servoTimer.resume();
}

void ZBServo::write(int angle) {
   // Inline constrain
   angle = (angle > 180) ? 180 : angle;

   // Direct calculation without float
   uint32_t pulse = SERVO_MIN_PULSE + ((SERVO_MAX_PULSE - SERVO_MIN_PULSE) * angle / 180);
   servoTimer.setCaptureCompare(servoChannel, pulse, MICROSEC_COMPARE_FORMAT);
}

// void SimpleServo::update() {
//   // Keep sending pulses for the current position with wider range
//   int pulseWidth = map(currentAngle, 0, 180, 544, 2400);
//   // digitalWrite(servoPin, HIGH);
//   // delayMicroseconds(pulseWidth);
//   // digitalWrite(servoPin, LOW);
//   // delay(20);  // Standard servo refresh rate
//   analogWriteFrequency(50);  // Đặt PWM tần số 50Hz
//   analogWrite(servoPin, pulseWidth);  // Gửi tín hiệu PWM đến chân servo
// }

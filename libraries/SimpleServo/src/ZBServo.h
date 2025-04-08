// SimpleServo.h
#ifndef SIMPLE_SERVO_H
#define SIMPLE_SERVO_H

#include <Arduino.h>
#include <HardwareTimer.h> // Include the HardwareTimer library

class ZBServo {
  private:
    int servoPin;
    int currentAngle;
    unsigned long lastPulseTime;
    uint32_t servoChannel;
    int SERVO_MIN_PULSE;
    int SERVO_MAX_PULSE;
    HardwareTimer servoTimer;
  public:
    ZBServo(); // Default pulse widths for 0 and 180 degrees
    void attach(int pin, int SERVO_MIN_PULSE = 544, int SERVO_MAX_PULSE = 2500);
    void write(int angle); // Added sendNow parameter
};

#endif
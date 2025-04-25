// OneWire direct GPIO for CH32V boards
// Final implementation with all required macros and types

#ifndef OneWire_Direct_GPIO_CH32V_h
#define OneWire_Direct_GPIO_CH32V_h

#include <Arduino.h>

// Define the IO register type for CH32V
typedef uint32_t IO_REG_TYPE;
#define IO_REG_BASE_ATTR
#define IO_REG_MASK_ATTR

// Define the pin to bitmask and register macros
// For CH32V, we'll store the pin number in baseReg and use it directly
#define PIN_TO_BITMASK(pin) ((uint32_t)1)
#define PIN_TO_BASEREG(pin) ((uint32_t)(pin))

// Define direct access macros using Arduino functions
// These use the pin number stored in baseReg
#define DIRECT_READ(base, mask) digitalRead((uint8_t)(base))
#define DIRECT_MODE_INPUT(base, mask) pinMode((uint8_t)(base), INPUT)
#define DIRECT_MODE_OUTPUT(base, mask) pinMode((uint8_t)(base), OUTPUT)
#define DIRECT_WRITE_LOW(base, mask) digitalWrite((uint8_t)(base), LOW)
#define DIRECT_WRITE_HIGH(base, mask) digitalWrite((uint8_t)(base), HIGH)

#endif
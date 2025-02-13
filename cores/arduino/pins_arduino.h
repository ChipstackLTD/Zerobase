/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef _PINS_ARDUINO_H_
#define _PINS_ARDUINO_H_
#include <stdbool.h>
#include <stdlib.h> /* Required for static_assert */
#include "variant.h"
#include "PinNames.h"

#include "pins_arduino_analog.h"
#include "pins_arduino_digital.h"

/*
 * Pin number mask
 * allows to retrieve the pin number without ALTx
 */
#define PNUM_MASK                   0xFF

/* Pin not defined */
#define PNUM_NOT_DEFINED            NUM_DIGITAL_PINS

/* Avoid PortName issue */
_Static_assert(LastPort <= 0x0F, "PortName must be less than 16");

_Static_assert(NUM_ANALOG_INPUTS <= MAX_ANALOG_INPUTS,
               "Core NUM_ANALOG_INPUTS limited to MAX_ANALOG_INPUTS");

/* Default for Arduino connector compatibility */
/* SPI Definitions */
#ifndef PIN_SPI_SS
  #define PIN_SPI_SS                10
#endif
#ifndef PIN_SPI_SS1
  #define PIN_SPI_SS1               4
#endif
#ifndef PIN_SPI_SS2
  #define PIN_SPI_SS2               7
#endif
#ifndef PIN_SPI_SS3
  #define PIN_SPI_SS3               8
#endif
#ifndef PIN_SPI_MOSI
  #define PIN_SPI_MOSI              11
#endif
#ifndef PIN_SPI_MISO
  #define PIN_SPI_MISO              12
#endif
#ifndef PIN_SPI_SCK
  #define PIN_SPI_SCK               13
#endif

static const uint32_t SS   = PIN_SPI_SS;
static const uint32_t SS1  = PIN_SPI_SS1;
static const uint32_t SS2  = PIN_SPI_SS2;
static const uint32_t SS3  = PIN_SPI_SS3;
static const uint32_t MOSI = PIN_SPI_MOSI;
static const uint32_t MISO = PIN_SPI_MISO;
static const uint32_t SCK  = PIN_SPI_SCK;

/* I2C2 Definitions */
#ifndef PIN_WIRE_SDA2
#define PIN_WIRE_SDA2 14
#endif
#ifndef PIN_WIRE_SCL2
#define PIN_WIRE_SCL2 15
#endif

// I2C1 definitions
#ifndef PIN_WIRE_SDA1
#define PIN_WIRE_SDA1 3
#endif
#ifndef PIN_WIRE_SCL1
#define PIN_WIRE_SCL1 2
#endif

static const uint32_t SDA2 = PIN_WIRE_SDA2;
static const uint32_t SCL2 = PIN_WIRE_SCL2;

static const uint32_t SDA1 = PIN_WIRE_SDA1;
static const uint32_t SCL1 = PIN_WIRE_SCL1;

#ifdef __cplusplus
extern "C" {
#endif
extern const PinName digitalPin[];
extern const uint32_t analogInputPin[];

#define NOT_AN_INTERRUPT            (uint32_t)NC

/* Convert a digital pin number Dxx to a PinName PX_n */
#if NUM_ANALOG_INPUTS > 0
/* Note: Analog pin is also a digital pin */
#define digitalPinToPinName(p)      ((((uint32_t)(p) & PNUM_MASK) < NUM_DIGITAL_PINS) ? \
            (PinName)(digitalPin[(uint32_t)(p) & PNUM_MASK] | ((p) & ALTX_MASK)) : \
            (((uint32_t)(p) & PNUM_ANALOG_BASE) == PNUM_ANALOG_BASE) && \
            (((uint32_t)(p) & PNUM_MASK) < NUM_ANALOG_INTERNAL_FIRST) ? \
            (PinName)(digitalPin[analogInputPin[(p) & PNUM_ANALOG_INDEX]] | ((p) & ALTX_MASK)) : NC)
#else
#define digitalPinToPinName(p)      ((((uint32_t)(p) & PNUM_MASK) < NUM_DIGITAL_PINS) ? \
            (PinName)(digitalPin[(uint32_t)(p) & PNUM_MASK] | ((p) & ALTX_MASK)) : NC)
#endif /* NUM_ANALOG_INPUTS > 0 */
/* Convert a PinName PX_n to a digital pin number */
uint32_t pinNametoDigitalPin(PinName p);

/* Convert an analog pin number to a digital pin number */
#if NUM_ANALOG_INPUTS > 0
/* Used by analogRead api to have A0 == 0 */
/* Non contiguous analog pins definition in digitalPin array */
#define analogInputToDigitalPin(p)  ((((uint32_t)(p) & PNUM_MASK) < NUM_ANALOG_INPUTS) ? \
            analogInputPin[(uint32_t)(p) & PNUM_MASK] | ((uint32_t)(p) & ALTX_MASK) : \
            (((uint32_t)(p) & PNUM_ANALOG_BASE) == PNUM_ANALOG_BASE) && \
            (((uint32_t)(p) & PNUM_MASK) < NUM_ANALOG_INTERNAL_FIRST) ? \
            analogInputPin[(p) & PNUM_ANALOG_INDEX] | ((uint32_t)(p) & ALTX_MASK) : (uint32_t)NC)
#else/* No analog pin defined */
#define analogInputToDigitalPin(p)  (NUM_DIGITAL_PINS)
#endif /* NUM_ANALOG_INPUTS > 0 */

/* Convert an analog pin number Ax to a PinName PX_n */
PinName analogInputToPinName(uint32_t pin);

/* All pins could manage EXTI */
#define digitalPinToInterrupt(p)    (digitalPinIsValid(p) ? p : NOT_AN_INTERRUPT)

#define digitalPinHasI2C(p)         (pin_in_pinmap(digitalPinToPinName(p), PinMap_I2C_SDA) ||\
                                     pin_in_pinmap(digitalPinToPinName(p), PinMap_I2C_SCL))
#define digitalPinHasPWM(p)         (pin_in_pinmap(digitalPinToPinName(p), PinMap_TIM))
#define digitalPinHasSerial(p)      (pin_in_pinmap(digitalPinToPinName(p), PinMap_UART_RX) ||\
                                     pin_in_pinmap(digitalPinToPinName(p), PinMap_UART_TX))
#define digitalPinHasSPI(p)         (pin_in_pinmap(digitalPinToPinName(p), PinMap_SPI_MOSI) ||\
                                     pin_in_pinmap(digitalPinToPinName(p), PinMap_SPI_MISO) ||\
                                     pin_in_pinmap(digitalPinToPinName(p), PinMap_SPI_SCLK) ||\
                                     pin_in_pinmap(digitalPinToPinName(p), PinMap_SPI_SSEL))


#define digitalPinToPort(p)         (get_GPIO_Port(CH_PORT(digitalPinToPinName(p))))
#define digitalPinToBitMask(p)      (CH_GPIO_PIN(digitalPinToPinName(p)))

#define analogInPinToBit(p)         (CH_GPIO_PIN(digitalPinToPinName(p)))
#define portOutputRegister(P)       (&(P->OUTDR))
#define portInputRegister(P)        (&(P->INDR))

#define portSetRegister(P)          (&(P->BSHR))
#define portClearRegister(P)        (&(P->BCR))


/*
 * Config registers split in 2 registers:
 * CFGLR: pin 0..7
 * CFGHR: pin 8..15
 * Return only CFGLR
 */
#define portModeRegister(P)         (&(P->CFGLR))
#define portConfigRegister(P)       (portModeRegister(P))


#define digitalPinIsValid(p)        (digitalPinToPinName(p) != NC)

/* As some pin could be duplicated in digitalPin[] */
/* return first occurrence of linked PinName (PYx) */
#define digitalPinFirstOccurence(p) (pinNametoDigitalPin(digitalPinToPinName(p)))

/* Specific for Firmata */
#if defined(PIN_SERIAL_RX) && defined(PIN_SERIAL_TX)
#define pinIsSerial(p)              ((digitalPinToPinName(p) == \
                                      digitalPinToPinName(PIN_SERIAL_RX & PNUM_MASK)) ||\
                                     (digitalPinToPinName(p) == \
                                      digitalPinToPinName(PIN_SERIAL_TX & PNUM_MASK)))
#endif
/* Convenient macro to handle Analog for Firmata */
#define pinIsAnalogInput digitalpinIsAnalogInput
bool digitalpinIsAnalogInput(uint32_t pin);
uint32_t digitalPinToAnalogInput(uint32_t pin);

#ifdef __cplusplus
}
#endif

/* Default Definitions, could be redefined in variant.h */
#ifndef ADC_RESOLUTION
  #define ADC_RESOLUTION            12
#endif

#ifndef DACC_RESOLUTION             
#define DACC_RESOLUTION             12
#endif

#ifndef PWM_RESOLUTION
  #define PWM_RESOLUTION            12
#endif

_Static_assert((ADC_RESOLUTION > 0) &&(ADC_RESOLUTION <= 32),
               "ADC_RESOLUTION must be 0 < x <= 32!");
_Static_assert((PWM_RESOLUTION > 0) &&(PWM_RESOLUTION <= 32),
               "PWM_RESOLUTION must be 0 < x <= 32!");

#ifndef PWM_FREQUENCY
  #define PWM_FREQUENCY             1000
#endif
#ifndef PWM_MAX_DUTY_CYCLE
  #define PWM_MAX_DUTY_CYCLE        4095
#endif

#endif /*_PINS_ARDUINO_H_*/

/*
 *******************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * This software component is licensed by WCH under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */
#pragma once

/* ENABLE Peripherals */
#define                         ADC_MODULE_ENABLED
#define                         UART_MODULE_ENABLED
#define                         SPI_MODULE_ENABLED
#define                         I2C_MODULE_ENABLED
#define                         TIM_MODULE_ENABLED

/* CH32V003F4 Pins */


/* Mapping to Digital Pins or Analog Pins */
#define PA1                     PIN_A1
#define PA2                     PIN_A0
#define PC0                     10
#define PC1                     18
#define PC2                     19
#define PC3                     2 
#define PC4                     PIN_A2
#define PC5                     13
#define PC6                     11
#define PC7                     12
#define PD0                     1
#define PD1                     0
#define PD2                     PIN_A3
#define PD6                     3 
#define PD3                     4 
#define PD4                     5 
#define PD5                     6 

// Alternate pins number
#define PD5_ALT1                (PD5  | ALT1)
#define PD6_ALT1                (PD6  | ALT1)


#define NUM_DIGITAL_PINS        22
#define NUM_ANALOG_INPUTS       8

// #define ADC_CTLR_ADCAL          
#define ADC_RESOLUTION          10



// On-board LED pin number 
#ifndef LED_BUILTIN
  #define LED_BUILTIN           2
#endif



// On-board user button 
#ifndef USER_BTN
  #define USER_BTN              3
#endif




// UART Definitions
// #ifndef SERIAL_UART_INSTANCE
//   #define SERIAL_UART_INSTANCE  1
// #endif
// Default pin used for generic 'Serial' instance
// Mandatory for Firmata
#ifndef PIN_SERIAL_RX
  #define PIN_SERIAL_RX         PD1
#endif
#ifndef PIN_SERIAL_TX
  #define PIN_SERIAL_TX         PD0
#endif

#define HAVE_HWSERIAL1

// SPI definitions
#ifndef PIN_SPI_SS
  #define PIN_SPI_SS            PC0
#endif
#ifndef PIN_SPI_MOSI
  #define PIN_SPI_MOSI          PC6
#endif
#ifndef PIN_SPI_MISO
  #define PIN_SPI_MISO          PC7
#endif
#ifndef PIN_SPI_SCK
  #define PIN_SPI_SCK           PC5
#endif

// I2C definitions
#ifndef PIN_WIRE_SDA2
  #define PIN_WIRE_SDA2          PC1
#endif
#ifndef PIN_WIRE_SCL2
  #define PIN_WIRE_SCL2          PC2
#endif

// Timer Definitions
#ifndef TIMER_TONE
  #define TIMER_TONE            TIM2
#endif
#ifndef TIMER_SERVO
  #define TIMER_SERVO           TIM1
#endif


/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus
  // These serial port names are intended to allow libraries and architecture-neutral
  // sketches to automatically default to the correct port name for a particular type
  // of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
  // the first hardware serial port whose RX/TX pins are not dedicated to another use.
  //
  // SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
  //
  // SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
  //
  // SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
  //
  // SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
  //
  // SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
  //                            pins are NOT connected to anything by default.
  #ifndef SERIAL_PORT_MONITOR
    #define SERIAL_PORT_MONITOR   Serial
  #endif
  #ifndef SERIAL_PORT_HARDWARE
    #define SERIAL_PORT_HARDWARE  Serial
  #endif
#endif



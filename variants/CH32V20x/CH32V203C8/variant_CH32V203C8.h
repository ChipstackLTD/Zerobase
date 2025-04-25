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
/* CH32V203C8 Pins */
#define PA9 1
#define PA10 0
#define PB6   2
#define PB7   3
#define PB8   4
#define PB9   5
#define PB12  6
#define PB13  7
#define PB14  8
#define PB15  9
#define PA15  10
#define PB5   11
#define PB4   12
#define PB3   13
#define PA0   PIN_A0
#define PA1   PIN_A1
#define PA2   PIN_A2
#define PA3   PIN_A3
#define PB11  18
#define PB10  19
#define PA4   PIN_A6
#define PA5   PIN_A7
#define PC14  22
#define PA6   23
#define PA7   24
#define PA8   25
#define PA11  26
#define PA12  27
#define PA13  28
#define PA14  29
#define PB0   30
#define PB1   31
#define PB2   32
#define PC13  33
#define PC15  34
#define PD0   35
#define PD1   36

// Alternate pins number
#define PA0_ALT1                (PA0  | ALT1)
#define PA1_ALT1                (PA1  | ALT1)
#define PA2_ALT1                (PA2  | ALT1)
#define PA3_ALT1                (PA3  | ALT1)
#define PA4_ALT1                (PA4  | ALT1)
#define PA5_ALT1                (PA5  | ALT1)
#define PA6_ALT1                (PA6  | ALT1)
#define PA7_ALT1                (PA7  | ALT1)
#define PA8_ALT1                (PA8  | ALT1)
#define PA9_ALT1                (PA9  | ALT1)
#define PA10_ALT1               (PA10 | ALT1)
#define PA11_ALT1               (PA11 | ALT1)
#define PA15_ALT1               (PA15 | ALT1)
#define PB0_ALT1                (PB0  | ALT1)
#define PB0_ALT2                (PB0  | ALT2)
#define PB1_ALT1                (PB1  | ALT1)
#define PB1_ALT2                (PB1  | ALT2)
#define PB3_ALT1                (PB3  | ALT1)
#define PB10_ALT1               (PB10 | ALT1)
#define PB11_ALT1               (PB11 | ALT1)

#define NUM_DIGITAL_PINS        38
#define NUM_ANALOG_INPUTS       10
// #define ADC_CTLR_ADCAL          
#define ADC_RESOLUTION          12


// On-board LED pin number
#ifndef LED_BUILTIN
  #define LED_BUILTIN           22
#endif

// On-board user button
#ifndef USER_BTN
  #define USER_BTN              PNUM_NOT_DEFINED
#endif

// SPI1 definitions
#ifndef PIN_SPI_SS
#define PIN_SPI_SS PA15
#endif
#ifndef PIN_SPI_MOSI
#define PIN_SPI_MOSI PB5
#endif
#ifndef PIN_SPI_MISO
#define PIN_SPI_MISO PB4
#endif
#ifndef PIN_SPI_SCK
#define PIN_SPI_SCK PB3
#endif

// SPI2 definitions
#ifndef PIN_SPI_SS1
#define PIN_SPI_SS1 PB12
#endif
#ifndef PIN_SPI_SCK1
#define PIN_SPI_SCK1 PB13
#endif
#ifndef PIN_SPI_MISO1
#define PIN_SPI_MISO1 PB14
#endif
#ifndef PIN_SPI_MOSI1
#define PIN_SPI_MOSI1 PB15
#endif

#ifndef PIN_SPI_SS2
#define PIN_SPI_SS2 PNUM_NOT_DEFINED
#endif
#ifndef PIN_SPI_SS3
#define PIN_SPI_SS3 PNUM_NOT_DEFINED
#endif

// I2C1 definitions
#ifndef PIN_WIRE_SDA1
#define PIN_WIRE_SDA1 PB7
#endif
#ifndef PIN_WIRE_SCL1
#define PIN_WIRE_SCL1 PB6
#endif

// I2C2 definitions
#ifndef PIN_WIRE_SDA2
#define PIN_WIRE_SDA2 PB11
#endif
#ifndef PIN_WIRE_SCL2
#define PIN_WIRE_SCL2 PB10
#endif

#ifndef SDA
#define SDA PIN_WIRE_SDA2
#endif

#ifndef SCL
#define SCL PIN_WIRE_SCL2
#endif

#ifndef DM
#define DM PB6
#endif

#ifndef DP
#define DP PB7
#endif

#ifndef CRX
#define CRX PB8
#endif

#ifndef CTX
#define CTX PB9
#endif

#ifndef MO
#define MO PIN_SPI_MOSI
#endif

#ifndef MI
#define MI PIN_SPI_MISO
#endif



// Timer Definitions
// Use TIM6/TIM7 when possible as servo and tone don't need GPIO output pin
#ifndef TIMER_TONE
  #define TIMER_TONE            TIM3
#endif
#ifndef TIMER_SERVO
  #define TIMER_SERVO           TIM2
#endif

// In the "UART Definitions" section:
#ifndef PIN_SERIAL_RX
  #define PIN_SERIAL_RX PA10
#endif
#ifndef PIN_SERIAL_TX
  #define PIN_SERIAL_TX PA9
#endif

#ifndef PIN_SERIAL2_RX
  #define PIN_SERIAL2_RX PA3 // D17
#endif
#ifndef PIN_SERIAL2_TX
  #define PIN_SERIAL2_TX PA2 // D16
#endif

#ifndef PIN_SERIAL_2_CTS
  #define PIN_SERIAL_2_CTS PA0
#endif

#ifndef PIN_SERIAL_2_RTS
  #define PIN_SERIAL_2_RTS PA1
#endif

#ifndef PIN_SERIAL_2_CK
  #define PIN_SERIAL_2_CK PA4 
#endif

#ifndef PIN_SERIAL3_RX
  #define PIN_SERIAL3_RX PB11 // D18
#endif
#ifndef PIN_SERIAL3_TX
  #define PIN_SERIAL3_TX PB10 // D19
#endif

// Enable Serial2 and Serial3 instances
#define HAVE_HWSERIAL1
#define HAVE_HWSERIAL2
#define HAVE_HWSERIAL3
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

/**
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

#include "pins_arduino.h"




// Digital PinName array
const PinName digitalPin[] = {
  PA_10,   // D0
  PA_9,  // D1
  PB_6,   // D2
  PB_7,   // D3
  PB_8,   // D4
  PB_9,   // D5
  PB_12,  // D6
  PB_13,  // D7
  PB_14,  // D8
  PB_15,  // D9
  PA_15,  // D10
  PB_5,   // D11
  PB_4,   // D12
  PB_3,   // D13
  PA_0,   // D14/A0
  PA_1,   // D15/A1
  PA_2,   // D16/A2
  PA_3,   // D17/A3
  PB_11,  // D18
  PB_10,  // D19
  PA_4,   // D20/A6
  PA_5,   // D21/A7
  PC_14,  // D22 --> This is for LED
  PA_6,   // D23/Unused
  PA_7,   // D24/Unused
  PA_8,   // D25/Unused
  PA_11,  // D26/Unused   
  PA_12,  // D27/Unused
  PA_13,  // D28/Unused
  PA_14,  // D29/Unused
  PB_0,   // D30/Unused
  PB_1,   // D31/Unused
  PB_2,   // D32/Unused
  PC_13,  // D33/Unused
  PC_15,  // D34/Unused
  PD_0,   // D35/Unused
  PD_1,   // D36/Unused
  NC      // D37 --> This is fake pin
};

// Analog (Ax) pin number array
const uint32_t analogInputPin[] = {
  14,  // A0,  PA0
  15,  // A1,  PA1
  16,  // A2,  PA2
  17,  // A3,  PA3
  37,  // A4,  NC --> so connect to any NC pin. 37 means NC in digitalPin (also fake pin)
  37,  // A5,  NC
  20,  // A6,  PA6
  21,  // A7,  PA7
  37, // A8,  NC
  37  // A9,  NC
};




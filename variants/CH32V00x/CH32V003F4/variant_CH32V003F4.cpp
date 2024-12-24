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




// Digital PinName array,Some GPIOs are bound to the same pin. 
const PinName digitalPin[] = {
  PD_1,   // D0   
  PD_0,   // D1
  PC_3,   // D2
  PD_6,   // D3
  /* PD_3, PD_4, PD_5 is for USB purpose */
  PD_3,   // D4 --> But not actually connected PD_3 to Arduino D4. Just for firmware only
  PD_4,   // D5 --> But not actually connected PD_4 to Arduino D5. Just for firmware only
  PD_5,   // D6 --> But not actually connected PD_5 to Arduino D6. Just for firmware only
  NC,     // D7
  NC,     // D8
  NC,     // D9
  PC_0,   // D10
  PC_6,   // D11
  PC_7,   // D12
  PC_5,   // D13
  PA_2,   // D14/A0
  PA_1,   // D15/A1
  PC_4,   // D16/A2 
  PD_2,   // D17/A3
  PC_1,   // D18
  PC_2,   // D19
  NC,     // D20
  NC      // D21
};

// Analog (Ax) pin number array
const uint32_t analogInputPin[] = {
  14,   // A0/PA2
  15,   // A1/PA1
  16,   // A2/PC4
  17,   // A3/PD2
  7,    // A4/NC --> so connect to any NC pin. 7 means NC in digitalPin
  7,    // A5/NC
  7,    // A6/NC
  7     // A7/NC 
};




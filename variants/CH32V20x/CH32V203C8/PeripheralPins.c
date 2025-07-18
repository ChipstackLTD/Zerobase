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

#include "Arduino.h"
#include "PeripheralPins.h"

/* =====
 * Notes:
 * - The pins mentioned Px_y_ALTz are alternative possibilities which use other
 *   HW peripheral instances. You can use them the same way as any other "normal"
 *   pin (i.e. analogWrite(PA7_ALT1, 128);).
 *
 * - Commented lines are alternative possibilities which are not used per default.
 *   If you change them, you will have to know what you do
 * =====
 */

//*** ADC ***
#ifdef ADC_MODULE_ENABLED
WEAK const PinMap PinMap_ADC[] = {
  {PA_0,      ADC1, CH_PIN_DATA_EXT(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, AFIO_NONE, 0)}, // ADC1_IN0
  {PA_0_ALT1, ADC2, CH_PIN_DATA_EXT(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, AFIO_NONE, 0)}, // ADC2_IN0
  {PA_1,      ADC1, CH_PIN_DATA_EXT(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, AFIO_NONE, 1)}, // ADC1_IN1
  {PA_1_ALT1, ADC2, CH_PIN_DATA_EXT(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, AFIO_NONE, 1)}, // ADC2_IN1
  {PA_2,      ADC1, CH_PIN_DATA_EXT(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, AFIO_NONE, 2)}, // ADC1_IN2
  {PA_2_ALT1, ADC2, CH_PIN_DATA_EXT(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, AFIO_NONE, 2)}, // ADC2_IN2
  {PA_3,      ADC1, CH_PIN_DATA_EXT(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, AFIO_NONE, 3)}, // ADC1_IN3
  {PA_3_ALT1, ADC2, CH_PIN_DATA_EXT(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, AFIO_NONE, 3)}, // ADC2_IN3
  {PA_4,      ADC1, CH_PIN_DATA_EXT(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, AFIO_NONE, 4)}, // ADC1_IN4
  {PA_4_ALT1, ADC2, CH_PIN_DATA_EXT(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, AFIO_NONE, 4)}, // ADC2_IN4
  {PA_5,      ADC1, CH_PIN_DATA_EXT(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, AFIO_NONE, 5)}, // ADC1_IN5
  {PA_5_ALT1, ADC2, CH_PIN_DATA_EXT(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, AFIO_NONE, 5)}, // ADC2_IN5
  {NC,        NP,   0}, // ADC1_IN4
  {NC,        NP,   0}, // ADC2_IN4
  {NC,        NP,   0}, // ADC1_IN5
  {NC,        NP,   0}, // ADC2_IN5
  {NC,        NP,   0}, // ADC1_IN8
  {NC,        NP,   0}, // ADC2_IN8
  {NC,        NP,   0}, // ADC1_IN9
  {NC,        NP,   0}, // ADC2_IN9
  {NC,        NP,   0}
};
#endif

//*** No DAC ***



//*** I2C ***
#ifdef I2C_MODULE_ENABLED
WEAK const PinMap PinMap_I2C_SDA[] = {
  {PB_7, I2C1, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFOD, PULLUP, AFIO_NONE)},
  {PB_11, I2C2, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFOD, PULLUP, AFIO_NONE)},
  {NC,   NP,   0}
};
#endif

#ifdef I2C_MODULE_ENABLED
WEAK const PinMap PinMap_I2C_SCL[] = {
  {PB_6, I2C1, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFOD, PULLUP, AFIO_NONE)},
  {PB_10, I2C2, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFOD, PULLUP, AFIO_NONE)},
  {NC,   NP,   0}
};
#endif

//*** TIM ***
#ifdef TIM_MODULE_ENABLED
WEAK const PinMap PinMap_TIM[] = {
  {PA_0,       TIM2, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM2_DISABLE, 1)}, // TIM2_CH1
  {PA_1,       TIM2, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM2_DISABLE, 2)}, // TIM2_CH2
  {PA_2,       TIM2, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM2_DISABLE, 3)}, // TIM2_CH3
  {PA_3,       TIM2, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM2_DISABLE, 4)}, // TIM2_CH4

  {PB_3,       TIM2, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Partial1Remap_TIM2_ENABLE, 2)}, // TIM2_CH2_1
  {PB_3,       TIM2, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_FullRemap_TIM2_ENABLE, 2)}, // TIM2_CH2_3
  {PB_10,       TIM2, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_FullRemap_TIM2_ENABLE, 3)}, // TIM2_CH3_3
  {PB_11,       TIM2, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Partial2Remap_TIM2_ENABLE, 4)}, // TIM2_CH4_2

  {PA_15,      TIM2, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Partial1Remap_TIM2_ENABLE, 1)}, // TIM2_CH1_1
  {PA_15,      TIM2, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Partial1Remap_TIM2_ENABLE, 9)}, // TIM2_ETR_1
  {PA_15,      TIM2, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_FullRemap_TIM2_ENABLE, 1)}, // TIM2_CH1_3
  {PA_15,      TIM2, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_FullRemap_TIM2_ENABLE, 9)}, // TIM2_ETR_3

  {PA_6,       TIM3, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM3_DISABLE, 1)}, // TIM3_CH1
  {PA_7,       TIM3, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM3_DISABLE, 2)}, // TIM3_CH2
  {PB_0,       TIM3, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM3_DISABLE, 3)}, // TIM3_CH3
  {PB_1,       TIM3, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM3_DISABLE, 4)}, // TIM3_CH4

  {PB_4,       TIM3, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_PartialRemap_TIM3_ENABLE, 1)}, // TIM3_CH1_2 Remap
  {PB_5,       TIM3, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_PartialRemap_TIM3_ENABLE, 2)}, // TIM3_CH2_2 Remap

  {PB_6,       TIM4, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM4_DISABLE, 1)}, // TIM4_CH1
  {PB_7,       TIM4, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM4_DISABLE, 2)}, // TIM4_CH2
  {PB_8,       TIM4, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM4_DISABLE, 3)}, // TIM4_CH3
  {PB_9,       TIM4, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM4_DISABLE, 4)}, // TIM4_CH4

  {PA_8,       TIM1, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM1_DISABLE, 1)}, // TIM1_CH1
  {PA_9,       TIM1, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM1_DISABLE, 2)}, // TIM1_CH2
  {PA_10,      TIM1, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM1_DISABLE, 3)}, // TIM1_CH3
  {PA_11,      TIM1, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM1_DISABLE, 4)}, // TIM1_CH4

  {PB_13,      TIM1, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM1_DISABLE, 1)}, // TIM1_CH1N
  {PB_14,      TIM1, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM1_DISABLE, 2)}, // TIM1_CH2N
  {PB_15,      TIM1, CH_PIN_DATA_EXT(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, NOPULL, AFIO_Remap_TIM1_DISABLE, 3)}, // TIM1_CH3N
  {NC,         NP,   0}
};
#endif

//*** UART ***
#ifdef UART_MODULE_ENABLED
WEAK const PinMap PinMap_UART_TX[] = {
  {PA_9, USART1, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_NONE)},
  {PA_2, USART2, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_NONE)},
  {PB_10,USART3, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_NONE)},
  {PB_0, UART4,  CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_NONE)},
  {NC,   NP,     0}
};
#endif

#ifdef UART_MODULE_ENABLED
WEAK const PinMap PinMap_UART_RX[] = {
  {PA_10, USART1, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_PUPD, PULLUP, AFIO_NONE)},
  {PA_3,  USART2, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_PUPD, PULLUP, AFIO_NONE)},
  {PB_11, USART3, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_PUPD, PULLUP, AFIO_NONE)},
  {PB_1,  UART4,  CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_PUPD, PULLUP, AFIO_NONE)},
  {NC,    NP,     0}
};
#endif

#ifdef UART_MODULE_ENABLED
WEAK const PinMap PinMap_UART_RTS[] = {
  {PA_12, USART1, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_NONE)},
  {PA_1,  USART2, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_NONE)},
  {PB_14, USART3, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_NONE)},
  {PB_4,  UART4,  CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_NONE)},  
  {NC,    NP,     0}
};
#endif

#ifdef UART_MODULE_ENABLED
WEAK const PinMap PinMap_UART_CTS[] = {
  {PA_11, USART1, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_PUPD, PULLUP, AFIO_NONE)},  
  {PA_0,  USART2, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_PUPD, PULLUP, AFIO_NONE)},
  {PB_13, USART3, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_PUPD, PULLUP, AFIO_NONE)},
  {PB_3,  UART4,  CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_PUPD, PULLUP, AFIO_NONE)},    
  {NC,    NP,     0}
};
#endif

#ifdef UART_MODULE_ENABLED
WEAK const PinMap PinMap_UART_CK[] = {
  {PA_4, USART2, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_NONE)},
};
#endif


//*** SPI ***
#ifdef SPI_MODULE_ENABLED
WEAK const PinMap PinMap_SPI_MOSI[] = {
  {PB_5, SPI1, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_Remap_SPI1_ENABLE)},
  {PB_15,SPI2, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_NONE)},
  {NC,   NP,   0}
};
#endif

#ifdef SPI_MODULE_ENABLED
WEAK const PinMap PinMap_SPI_MISO[] = {
  {PB_4, SPI1, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_FLOAT, 0, AFIO_Remap_SPI1_ENABLE)},
  {PB_14, SPI2, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_FLOAT, 0, AFIO_NONE)},
  {NC,   NP,   0}
};
#endif

#ifdef SPI_MODULE_ENABLED
WEAK const PinMap PinMap_SPI_SCLK[] = {
  {PB_3, SPI1, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_Remap_SPI1_ENABLE)},
  {PB_13, SPI2, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_NONE)},
  {NC,   NP,   0}
};
#endif

#ifdef SPI_MODULE_ENABLED
WEAK const PinMap PinMap_SPI_SSEL[] = {
  {PA_15,  SPI1, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_Remap_SPI1_ENABLE)},
  {PB_12,  SPI2, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_AFPP, 0, AFIO_NONE)},
  {NC,    NP,   0}
};
#endif

//*** CAN ***
#ifdef CAN_MODULE_ENABLED
WEAK const PinMap PinMap_CAN_RD[] = {
  {PB_8,  CAN1, CH_PIN_DATA(CH_MODE_INPUT, GPIO_NOPULL, AFIO_CAN1_2)},
  {NC,    NP,   0}
};
#endif

#ifdef CAN_MODULE_ENABLED 
WEAK const PinMap PinMap_CAN_TD[] = {
  {PB_9,  CAN1, CH_PIN_DATA(CH_MODE_AF_PP, GPIO_NOPULL, AFIO_CAN1_2)},
  {NC,    NP,   0}
};
#endif

//*** No ETHERNET ***



//*** USB ***
#ifdef USB_MODULE_ENABLED
WEAK const PinMap PinMap_USB[] = {
  {PA_11, USB, CH_PIN_DATA(CH_MODE_INPUT, GPIO_NOPULL, AFIO_NONE)}, // USB_DM
  {PA_12, USB, CH_PIN_DATA(CH_MODE_INPUT, GPIO_NOPULL, AFIO_NONE)}, // USB_DP
  {NC,    NP,  0}
};
#endif

//*** No SD ***



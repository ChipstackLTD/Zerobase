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

#include "Arduino.h"

#include "PinConfigured.h"


#ifdef __cplusplus
extern "C" {
#endif


extern uint32_t g_anOutputPinConfigured[MAX_NB_PORT];

// void pinMode(uint32_t ulPin, uint32_t ulMode)
// {
//   PinName p = digitalPinToPinName(ulPin);

//   if (p != NC) {
//     // If the pin that support PWM or DAC output, we need to turn it off
// // #if (defined(DAC_MODULE_ENABLED) && !defined(DAC_MODULE_ONLY)) ||\
// //     (defined(TIM_MODULE_ENABLED) && !defined(TIM_MODULE_ONLY))
// //     if (is_pin_configured(p, g_anOutputPinConfigured)) {
// // #if defined(DAC_MODULE_ENABLED) && !defined(DAC_MODULE_ONLY)
// //       if (pin_in_pinmap(p, PinMap_DAC)) {
// //         dac_stop(p);
// //       } else
// // #endif //DAC_MODULE_ENABLED && !DAC_MODULE_ONLY
// // #if defined(TIM_MODULE_ENABLED) && !defined(TIM_MODULE_ONLY)
// //         if (pin_in_pinmap(p, PinMap_TIM)) {
// //           pwm_stop(p);
// //         }
// // #endif //TIM_MODULE_ENABLED && !TIM_MODULE_ONLY
// //       {
// //         reset_pin_configured(p, g_anOutputPinConfigured);
// //       }
// //     }
// // #endif
//     // if(ulMode == OUTPUT || ulMode == OUTPUT_OD){
//     //   if(p == PD_1){
//     //     pinV32_DisconnectDebug(PD_1);
//     //   }
//     // }
//     // static const uint32_t pinModeTable[] = {
//     //     CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_FLOAT, 0, 0),  
//     //     CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_PP, 0, 0),  
//     //     CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_PUPD, 0x1, 0),  
//     //     CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_PUPD, 0x2, 0),  
//     //     CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, 0),  
//     //     CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_OD, 0, 0)  
//     // };

//     // pin_function(p, pinModeTable[ulMode]);

//     switch (ulMode)  /* INPUT_FLOATING */
// {
//   case INPUT: 
//     pin_function(p, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_FLOAT, 0, 0));
//     break;
//   case OUTPUT:
//     if(p == PD_1){
//       pinV32_DisconnectDebug(PD_1);
//     }
//     pin_function(p, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_PP, 0, 0));
//     break;
//   case INPUT_PULLUP:
//     pin_function(p, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_PUPD, 0x1, 0));
//     break;
//   case INPUT_PULLDOWN:
//     pin_function(p, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_PUPD, 0x2, 0));
//     break;
//   case INPUT_ANALOG:
//     pin_function(p, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_ANALOG, 0, 0));
//     break;
//   case OUTPUT_OD:
//     pin_function(p, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_OD, 0,0));
//     break;
//   default:
//     Error_Handler();
//     break;
// }
//   }
// }

void pinMode(uint32_t ulPin, uint32_t ulMode)
{
  PinName p = digitalPinToPinName(ulPin);

  if (p != NC)
  {
    /* Get the pin information */
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    /* Enable GPIO clock */
    uint32_t port = CH_PORT(p);
    GPIO_TypeDef *gpio = set_GPIO_Port_Clock(port);
    uint32_t ch_pinx = CH_MAP_GPIO_PIN(p);

    switch (ulMode) /* INPUT_FLOATING */
    {

    case INPUT:
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
      break;
    case OUTPUT:
      if (p == PD_1)
      {
        pinV32_DisconnectDebug(PD_1);
      }
      GPIO_InitStructure.GPIO_Speed = CH_MODE_OUTPUT_50MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
      break;
    case INPUT_PULLUP:
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
      break;
    case INPUT_PULLDOWN:
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
      break;
    case INPUT_ANALOG:
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
      break;
    case OUTPUT_OD:
      GPIO_InitStructure.GPIO_Speed = CH_MODE_OUTPUT_50MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
      break;
    default:
      Error_Handler();
      break;
    }
    GPIO_InitStructure.GPIO_Pin = (uint16_t)ch_pinx;
    uint32_t pin = CH_PIN(p); // Get pin index 0–15
    uint32_t currentmode = GPIO_InitStructure.GPIO_Mode & 0x0F;

    if ((GPIO_InitStructure.GPIO_Mode & 0x10) != 0)
    {
        currentmode |= GPIO_InitStructure.GPIO_Speed;
    }

    uint32_t pos = (pin % 8) << 2;        // Position within register
    uint32_t pinmask = 0x0F << pos;

    if (pin < 8)
    {
        uint32_t tmpreg = gpio->CFGLR;
        tmpreg &= ~pinmask;
        tmpreg |= (currentmode << pos);
        gpio->CFGLR = tmpreg;
    }
    else
    {
        uint32_t tmpreg = gpio->CFGHR;
        tmpreg &= ~pinmask;
        tmpreg |= (currentmode << pos);
        gpio->CFGHR = tmpreg;
    }

    // Set pull-up/down using BCR or BSHR
    if (GPIO_InitStructure.GPIO_Mode == GPIO_Mode_IPD)
    {
        gpio->BCR = 1 << pin;
    }
    else if (GPIO_InitStructure.GPIO_Mode == GPIO_Mode_IPU)
    {
        gpio->BSHR = 1 << pin;
    }
  }
}

void digitalWrite(uint32_t ulPin, uint32_t ulVal)
{
  digitalWriteFast(digitalPinToPinName(ulPin), ulVal);
}

int digitalRead(uint32_t ulPin)
{
  return digitalReadFast(digitalPinToPinName(ulPin));
}

void digitalToggle(uint32_t ulPin)
{
  digitalToggleFast(digitalPinToPinName(ulPin));
}

#ifdef __cplusplus
}
#endif

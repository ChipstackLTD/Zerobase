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
#include "analog.h"
#include "ch32yyxx_adc.h"
#include "PinAF_ch32yyxx.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Private_Variables */
#if (defined(ADC_MODULE_ENABLED) && !defined(ADC_MODULE_ONLY)) ||\
    (defined(DAC_MODULE_ENABLED) && !defined(DAC_MODULE_ONLY))
static PinName g_current_pin = NC;
static int calibration_value = 0;
#endif


/* Private_Defines */
#if defined(ADC_MODULE_ENABLED) && !defined(ADC_MODULE_ONLY)

#if (defined(CH32V20x) || defined(CH32V30x) || defined(CH32V30x_C) || defined(CH32V10x) || defined(CH32L10x) || defined(CH32VM00X) )

/* Default to use maximum sampling period */
#if defined(CH32L10x) || defined(CH32VM00X)

#ifndef ADC_SAMPLINGTIME
#if defined(ADC_SampleTime_CyclesMode7) 
#define ADC_SAMPLINGTIME        ADC_SampleTime_CyclesMode7
#elif defined(ADC_SampleTime_CyclesMode6)
#define ADC_SAMPLINGTIME        ADC_SampleTime_CyclesMode6
#elif defined(ADC_SampleTime_CyclesMode5)
#define ADC_SAMPLINGTIME        ADC_SampleTime_CyclesMode5
#elif defined(ADC_SampleTime_CyclesMode4)
#define ADC_SAMPLINGTIME        ADC_SampleTime_CyclesMode4
#elif defined(ADC_SampleTime_CyclesMode3)
#define ADC_SAMPLINGTIME        ADC_SampleTime_CyclesMode3
#elif defined(ADC_SampleTime_CyclesMode2)
#define ADC_SAMPLINGTIME        ADC_SampleTime_CyclesMode2
#elif defined(ADC_SampleTime_CyclesMode1)
#define ADC_SAMPLINGTIME        ADC_SampleTime_CyclesMode1
#elif defined(ADC_SampleTime_CyclesMode0)
#define ADC_SAMPLINGTIME        ADC_SampleTime_CyclesMode0
#endif
#endif /* !ADC_SAMPLINGTIME */

#else

#ifndef ADC_SAMPLINGTIME
#if defined(ADC_SampleTime_239Cycles5) 
#define ADC_SAMPLINGTIME        ADC_SampleTime_239Cycles5
#elif defined(ADC_SampleTime_71Cycles5)
#define ADC_SAMPLINGTIME        ADC_SampleTime_71Cycles5
#elif defined(ADC_SampleTime_55Cycles5)
#define ADC_SAMPLINGTIME        ADC_SampleTime_55Cycles5
#elif defined(ADC_SampleTime_41Cycles5)
#define ADC_SAMPLINGTIME        ADC_SampleTime_41Cycles5
#elif defined(ADC_SampleTime_28Cycles5)
#define ADC_SAMPLINGTIME        ADC_SampleTime_28Cycles5
#elif defined(ADC_SampleTime_13Cycles5)
#define ADC_SAMPLINGTIME        ADC_SampleTime_13Cycles5
#elif defined(ADC_SampleTime_7Cycles5)
#define ADC_SAMPLINGTIME        ADC_SampleTime_7Cycles5
#elif defined(ADC_SampleTime_1Cycles5)
#define ADC_SAMPLINGTIME        ADC_SampleTime_1Cycles5
#endif
#endif /* !ADC_SAMPLINGTIME */

#endif /* CH32L10x */


/*
 * Default to use maximum sampling period 
 */

#if defined(CH32L10x) || defined(CH32VM00X)

#ifndef ADC_SAMPLINGTIME_INTERNAL
#if defined(ADC_SampleTime_CyclesMode7)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_CyclesMode7
#elif defined(ADC_SampleTime_CyclesMode6)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_CyclesMode6
#elif defined(ADC_SampleTime_CyclesMode5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_CyclesMode5
#elif defined(ADC_SampleTime_CyclesMode4)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_CyclesMode4
#elif defined(ADC_SampleTime_CyclesMode3)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_CyclesMode3
#elif defined(ADC_SampleTime_CyclesMode2)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_CyclesMode2
#elif defined(ADC_SampleTime_CyclesMode1)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_CyclesMode1
#elif defined(ADC_SampleTime_CyclesMode0)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_CyclesMode0
#else
#error "ADC sampling time could not be defined for internal channels!"
#endif
#endif /* !ADC_SAMPLINGTIME_INTERNAL */

#else

#ifndef ADC_SAMPLINGTIME_INTERNAL
#if defined(ADC_SampleTime_239Cycles5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_239Cycles5
#elif defined(ADC_SampleTime_71Cycles5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_71Cycles5
#elif defined(ADC_SampleTime_55Cycles5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_55Cycles5
#elif defined(ADC_SampleTime_41Cycles5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_41Cycles5
#elif defined(ADC_SampleTime_28Cycles5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_28Cycles5
#elif defined(ADC_SampleTime_13Cycles5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_13Cycles5
#elif defined(ADC_SampleTime_7Cycles5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_7Cycles5
#elif defined(ADC_SampleTime_1Cycles5)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_1Cycles5
#else
#error "ADC sampling time could not be defined for internal channels!"
#endif
#endif /* !ADC_SAMPLINGTIME_INTERNAL */


#endif



#ifndef ADC_CLOCK_DIV
#ifdef RCC_PCLK2_Div8
#define ADC_CLOCK_DIV       RCC_PCLK2_Div8
#elif defined(RCC_PCLK2_Div6) 
#define ADC_CLOCK_DIV       RCC_PCLK2_Div6
#elif defined(RCC_PCLK2_Div4)
#define ADC_CLOCK_DIV       RCC_PCLK2_Div4
#elif defined(RCC_PCLK2_Div2)
#define ADC_CLOCK_DIV       RCC_PCLK2_Div2
#endif
#endif /* !ADC_CLOCK_DIV */

#elif defined(CH32V00x)

/* Default to use maximum sampling period */
#ifndef ADC_SAMPLINGTIME
#if defined(ADC_SampleTime_241Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_241Cycles;
#elif defined(ADC_SampleTime_73Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_73Cycles;
#elif defined(ADC_SampleTime_57Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_57Cycles;
#elif defined(ADC_SampleTime_43Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_43Cycles;
#elif defined(ADC_SampleTime_30Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_30Cycles;
#elif defined(ADC_SampleTime_15Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_15Cycles;
#elif defined(ADC_SampleTime_9Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_9Cycles;
#elif defined(ADC_SampleTime_3Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_3Cycles
#endif
#endif /* !ADC_SAMPLINGTIME */

/*
 * Default to use maximum sampling period 
 */
#ifndef ADC_SAMPLINGTIME_INTERNAL
#if defined(ADC_SampleTime_241Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_241Cycles
#elif defined(ADC_SampleTime_73Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_73Cycles
#elif defined(ADC_SampleTime_57Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_57Cycles
#elif defined(ADC_SampleTime_43Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_43Cycles
#elif defined(ADC_SampleTime_30Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_30Cycles
#elif defined(ADC_SampleTime_15Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_15Cycles
#elif defined(ADC_SampleTime_9Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_9Cycles
#elif defined(ADC_SampleTime_3Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_3Cycles
#else
#error "ADC sampling time could not be defined for internal channels!"
#endif
#endif /* !ADC_SAMPLINGTIME_INTERNAL */

#ifndef ADC_CLOCK_DIV
#ifdef RCC_PCLK2_Div8
#define ADC_CLOCK_DIV       RCC_PCLK2_Div8
#elif defined(RCC_PCLK2_Div6) 
#define ADC_CLOCK_DIV       RCC_PCLK2_Div6
#elif defined(RCC_PCLK2_Div4)
#define ADC_CLOCK_DIV       RCC_PCLK2_Div4
#elif defined(RCC_PCLK2_Div2)
#define ADC_CLOCK_DIV       RCC_PCLK2_Div2
#endif
#endif /* !ADC_CLOCK_DIV */


#elif defined(CH32X035)



/* Default to use maximum sampling period */
#ifndef ADC_SAMPLINGTIME
#if defined(ADC_SampleTime_11Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_11Cycles;
#elif defined(ADC_SampleTime_10Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_10Cycles;
#elif defined(ADC_SampleTime_9Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_9Cycles;
#elif defined(ADC_SampleTime_8Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_8Cycles;
#elif defined(ADC_SampleTime_7Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_7Cycles;
#elif defined(ADC_SampleTime_6Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_6Cycles;
#elif defined(ADC_SampleTime_5Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_5Cycles;
#elif defined(ADC_SampleTime_4Cycles)
#define ADC_SAMPLINGTIME        ADC_SampleTime_4Cycles
#endif
#endif /* !ADC_SAMPLINGTIME */

/*
 * Default to use maximum sampling period 
 */
#ifndef ADC_SAMPLINGTIME_INTERNAL
#if defined(ADC_SampleTime_11Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_11Cycles
#elif defined(ADC_SampleTime_10Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_10Cycles
#elif defined(ADC_SampleTime_9Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_9Cycles
#elif defined(ADC_SampleTime_8Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_8Cycles
#elif defined(ADC_SampleTime_7Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_7Cycles
#elif defined(ADC_SampleTime_6Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_6Cycles
#elif defined(ADC_SampleTime_5Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_5Cycles
#elif defined(ADC_SampleTime_4Cycles)
#define ADC_SAMPLINGTIME_INTERNAL ADC_SampleTime_4Cycles
#else
#error "ADC sampling time could not be defined for internal channels!"
#endif
#endif /* !ADC_SAMPLINGTIME_INTERNAL */

#ifndef ADC_CLOCK_DIV
#ifdef ADC_CLK_Div8
#define ADC_CLOCK_DIV       ADC_CLK_Div8
#elif defined(ADC_CLK_Div6) 
#define ADC_CLOCK_DIV       ADC_CLK_Div6
#elif defined(ADC_CLK_Div5)
#define ADC_CLOCK_DIV       ADC_CLK_Div5
#elif defined(ADC_CLK_Div4)
#define ADC_CLOCK_DIV       ADC_CLK_Div4
#endif
#endif /* !ADC_CLOCK_DIV */




#endif /* !CH32V00x */


#ifndef ADC_REGULAR_RANK_1
#define ADC_REGULAR_RANK_1  1
#endif

/* Exported Functions */
/**
  * @brief  Return ADC HAL channel linked to a PinName
  * @param  pin: PinName
  * @retval Valid HAL channel
  */
uint32_t get_adc_channel(PinName pin)
{
  uint32_t function = pinmap_function(pin, PinMap_ADC);
  uint32_t channel = 0;
  switch (CH_PIN_CHANNEL(function)) {
#ifdef ADC_Channel_0
    case 0:
      channel = ADC_Channel_0;
      break;
#endif
    case 1:
      channel = ADC_Channel_1;
      break;
    case 2:
      channel = ADC_Channel_2;
      break;
    case 3:
      channel = ADC_Channel_3;
      break;
    case 4:
      channel = ADC_Channel_4;
      break;
    case 5:
      channel = ADC_Channel_5;
      break;
    case 6:
      channel = ADC_Channel_6;
      break;
    case 7:
      channel = ADC_Channel_7;
      break;
    case 8:
      channel = ADC_Channel_8;
      break;
#ifdef ADC_Channel_9  
    case 9:
      channel = ADC_Channel_9;
      break;
#endif
#ifdef ADC_Channel_10      
    case 10:
      channel = ADC_Channel_10;
      break;   
#endif
#ifdef ADC_Channel_11      
    case 11:
      channel = ADC_Channel_11;
      break;
#endif   
#ifdef ADC_Channel_12      
    case 12:
      channel = ADC_Channel_12;
      break;
    case 13:
      channel = ADC_Channel_13;
      break;
    case 14:
      channel = ADC_Channel_14;
      break;
    case 15:
      channel = ADC_Channel_15;
      break;
 #endif     
#ifdef ADC_Channel_16
    case 16:
      channel = ADC_Channel_16;
      break;
#endif
#ifdef ADC_Channel_17
    case 17:
      channel = ADC_Channel_17;
      break;
#endif
#ifdef ADC_Channel_18
    case 18:
      channel = ADC_Channel_18;
      break;
#endif
#ifdef ADC_Channel_19
    case 19:
      channel = ADC_Channel_19;
      break;
#endif
    default:
      _Error_Handler("ADC: Unknown adc channel", (int)(CH_PIN_CHANNEL(function)));
      break;
  }
  
  return channel;
}

/**
  * @brief  Return ADC HAL internal channel linked to a PinName
  * @param  pin: specific PinName's for ADC internal. Value can be:
  *         PADC_TEMP, PADC_TEMP_ADC5, PADC_VREF, PADC_VBAT
  *         Note that not all of these values ​​may be available for all series.
  * @retval Valid HAL internal channel.
  */
uint32_t get_adc_internal_channel(PinName pin)
{
  uint32_t channel = 0;
  switch (pin) {
#if defined(ADC_Channel_TempSensor)
    case PADC_TEMP:
      channel = ADC_Channel_TempSensor;
      break;
#endif
#ifdef ADC_Channel_Vrefint
    case PADC_VREF:
      channel = ADC_Channel_Vrefint;
      break;
#endif
#ifdef ADC_Channel_Vbat
    case PADC_VBAT:
      channel = ADC_CHANNEL_VBAT;
      break;
#endif
    default:
      _Error_Handler("ADC: Unknown adc internal PiName", (int)(pin));
      break;
  }
  return channel;
}
#endif /* ADC_MODULE_ENABLED && !ADC_MODULE_ONLY */



#if defined(DAC_MODULE_ENABLED) && !defined(DAC_MODULE_ONLY)
/**
  * @brief  Return DAC HAL channel linked to a PinName
  * @param  pin: specific PinName's for ADC internal.
  * @retval Valid  channel
  */
uint32_t get_dac_channel(PinName pin)
{
  uint32_t function = pinmap_function(pin, PinMap_DAC);
  uint32_t channel = 0;
  switch (CH_PIN_CHANNEL(function)) 
  {
#ifdef DAC_Channel_1
    case 1:
      channel = DAC_Channel_1;
      break;
#endif
#ifdef DAC_Channel_2
    case 2:
      channel = DAC_Channel_2;
      break;
#endif
    default:
      _Error_Handler("DAC: Unknown dac channel", (int)(CH_PIN_CHANNEL(function)));
      break;
  }
  return channel;
}

////////////////////////// DAC INTERFACE FUNCTIONS /////////////////////////////
/**
  * @brief DAC  Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hdac: DAC pointer
  * @retval None
  */
void DAC_Clock_EN(DAC_TypeDef *dac)
{
    /* DAC Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    /* Configure DAC GPIO pins */
    pinmap_pinout(g_current_pin, PinMap_DAC);
}


/**
  * @brief  This function will set the DAC to the required value
  * @param  port : the gpio port to use
  * @param  pin : the gpio pin to use
  * @param  value : the value to push on the adc output
  * @param  do_init : if set to 1 the initialization of the adc is done
  * @retval None
  */
void dac_write_value(PinName pin, uint32_t value, uint8_t do_init)
{
  DAC_TypeDef *pdac=NULL;
  DAC_InitTypeDef  DAC_InitType = {0};

  uint32_t dacChannel;

  pdac = (DAC_TypeDef *)pinmap_peripheral(pin, PinMap_DAC);
  if (pdac == NP) {
    return;
  }
  dacChannel = get_dac_channel(pin);

  if (do_init == 1) 
  {
   #if defined(CH32L10x)
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE );
	  RCC_PB1PeriphClockCmd(RCC_PB1Periph_DAC, ENABLE );   
   #else
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );
   #endif
    /*##-1- Configure the DAC peripheral */
    g_current_pin = pin;

    DAC_InitType.DAC_Trigger = DAC_Trigger_None;
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

    /*##-2- Configure DAC channelx */
    DAC_Init(dacChannel, &DAC_InitType);
    
  }
  /*##-3- Set DAC Channelx DHR register */
    switch (dacChannel)
    {
      case DAC_Channel_1:
        DAC_SetChannel1Data(DAC_Align_12b_R, value);
        break;
     case DAC_Channel_2:
        DAC_SetChannel2Data(DAC_Align_12b_R, value);
        break;
     default:
        DAC_SetChannel1Data(DAC_Align_12b_R, 0);
        DAC_SetChannel2Data(DAC_Align_12b_R, 0);
        break;
    }

  /*##-4- Enable DAC Channelx*/
  DAC_Cmd(dacChannel, ENABLE);
}


/**
  * @brief  This function will stop the DAC
  * @param  port : the gpio port to use
  * @param  pin : the gpio pin to use
  * @retval None
  */
void dac_stop(PinName pin)
{
  DAC_TypeDef *pdac=NULL;
  uint32_t dacChannel;

  pdac = (DAC_TypeDef *)pinmap_peripheral(pin, PinMap_DAC);
  if (pdac == NP) {
    return;
  }
  dacChannel = get_dac_channel(pin);
  //disable channel
  DAC_Cmd(dacChannel, DISABLE);

  DAC_DeInit( );
}
#endif //DAC_MODULE_ENABLED && !DAC_MODULE_ONLY




#if defined(ADC_MODULE_ENABLED) && !defined(ADC_MODULE_ONLY)
////////////////////////// ADC INTERFACE FUNCTIONS /////////////////////////////

/**
  * @brief ADC Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param padc: ADC handle pointer
  * @retval None
  */
void ADC_Clock_EN(ADC_TypeDef *padc)
{
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* ADC Periph clock enable */
  if (padc == ADC1) 
  {
     #ifdef RCC_APB2Periph_ADC1
      	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1	, ENABLE );	  //ENABLE ADC1 Clock
     #endif 

     #ifdef RCC_PB2Periph_ADC1
     RCC_PB2PeriphClockCmd(RCC_PB2Periph_ADC1	, ENABLE );	  //ENABLE ADC1 Clock for CH32L10x
     #endif
  }
#ifdef ADC2
  else if(padc == ADC2) 
  {
      #ifdef RCC_APB2Periph_ADC2
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2  , ENABLE );   //ENABLE ADC2 Clock 
      #endif 

      #ifdef RCC_PB2Periph_ADC2
      RCC_PB2PeriphClockCmd(RCC_PB2Periph_ADC2  , ENABLE );   //ENABLE ADC2 Clock for CH32L10x      
      #endif
  }
#endif
  /* Configure ADC GPIO pin */
  if (!(g_current_pin & PADC_BASE)) 
  {
      pinmap_pinout(g_current_pin, PinMap_ADC);
  }
}


/**
  * @brief  DeInitializes the ADC Moudle.
  * @param  padc: ADC handle
  * @retval None
  */
void ADC_Stop(ADC_TypeDef *padc)
{
    ADC_Cmd(padc,DISABLE); 
}

/**
  * @brief  This function will set the ADC to the required value
  * @param  pin : the pin to use
  * @param  resolution : resolution for converted data: 8/10/12
  * @retval the value of the adc
  */
uint16_t adc_read_value(PinName pin, uint32_t resolution)
{

  ADC_TypeDef *padc ;
  ADC_InitTypeDef ADC_InitStructure={0};

  __IO uint16_t uhADCxConvertedValue = 0;
  uint32_t samplingTime = ADC_SAMPLINGTIME;
  uint32_t channel = 0;
  uint32_t bank = 0;

  if ((pin & PADC_BASE) && (pin < ANA_START))  //internal channle 
  {
    channel = get_adc_internal_channel(pin);
    samplingTime = ADC_SAMPLINGTIME_INTERNAL;
    padc = ADC1;
  } 
  else 
  {
    padc = (ADC_TypeDef *)pinmap_peripheral(pin, PinMap_ADC);
    channel = get_adc_channel(pin);
  }

  g_current_pin = pin;    
  ADC_Clock_EN(padc);  

#ifdef ADC_CLOCK_DIV
 #if !defined(CH32X035) 
  RCC_ADCCLKConfig(ADC_CLOCK_DIV);            /* (A)synchronous clock mode, input ADC clock divided */
 #else
  ADC_CLKConfig(padc, ADC_CLOCK_DIV);
 #endif 
#endif
#ifdef ADC_RESOLUTION_12B
#else
  (void) resolution;
#endif
#ifdef ADC_DataAlign_Right
  ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;           /* Right-alignment for converted data */
#endif
  ADC_InitStructure.ADC_ScanConvMode         = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
 
  ADC_InitStructure.ADC_Mode                 = ADC_Mode_Independent;          /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
  ADC_InitStructure.ADC_ContinuousConvMode   = DISABLE;
  ADC_InitStructure.ADC_NbrOfChannel         = 1; 
  ADC_InitStructure.ADC_ExternalTrigConv     = ADC_ExternalTrigConv_None;
 #if !defined(CH32V00x) && !defined(CH32V10x)  && !defined(CH32VM00X)
  ADC_InitStructure.ADC_OutputBuffer         = ENABLE;
#endif
  ADC_Init(padc, &ADC_InitStructure);
  padc->STATR = 0;
  ADC_Cmd(padc,ENABLE); 

  /*##-2- Configure ADC regular channel ######################################*/  
  ADC_RegularChannelConfig(padc, channel, ADC_REGULAR_RANK_1, samplingTime );	  

#if defined(ADC_CTLR_ADCAL) 
  /*##-2.1- Calibrate ADC then Start the conversion process ####################*/
   #if !defined(CH32V00x) 
  ADC_BufferCmd(padc, DISABLE);   //disable buffer
  #endif
	ADC_ResetCalibration(padc);
	while(ADC_GetResetCalibrationStatus(padc));
	ADC_StartCalibration(padc);
	while(ADC_GetCalibrationStatus(padc));
	calibration_value = Get_CalibrationValue(padc);	
	ADC_BufferCmd(padc, ENABLE);   //enable buffer  
#endif

  /*##-3- Start the conversion process ####################*/
  ADC_SoftwareStartConvCmd(padc, ENABLE);
  /*##-4- Wait for the end of conversion #####################################*/
  /*  For simplicity reasons, this example is just waiting till the end of the
      conversion, but application may perform other tasks while conversion
      operation is ongoing. */
  while(!ADC_GetFlagStatus(padc, ADC_FLAG_EOC ));
  uhADCxConvertedValue = padc->RDATAR;
  ADC_Stop(padc);
  ADC_DeInit(padc);

#if defined(ADC_CTLR_ADCAL)
  #if (ADC_RESOLUTION == 8)
    if(( calibration_value + uhADCxConvertedValue ) >= 255 )
    {
      return 255;
    }
    else if( ( calibration_value + uhADCxConvertedValue ) <= 0 )
    {
      return 0;
    }
    else
    {       
      return (uhADCxConvertedValue+calibration_value);
    }
  #endif
    #if (ADC_RESOLUTION == 10)
    if(( calibration_value + uhADCxConvertedValue ) >= 1023 )
    {
      return 1023;
    }
    else if( ( calibration_value + uhADCxConvertedValue ) <= 0 )
    {
      return 0;
    }
    else
    {       
      return (uhADCxConvertedValue+calibration_value);
    }
  #endif  
  #if (ADC_RESOLUTION == 12)
    if(( calibration_value + uhADCxConvertedValue ) >= 4095 )
    {
      return 4095;
    }
    else if( ( calibration_value + uhADCxConvertedValue ) <= 0 )
    {
      return 0;
    }
    else       
    {
      return (uhADCxConvertedValue+calibration_value);
    }
  #endif
#else
  return uhADCxConvertedValue;
#endif
}
#endif /* ADC_MODULE_ENABLED && !ADC_MODULE_ONLY*/




#if defined(TIM_MODULE_ENABLED) && !defined(TIM_MODULE_ONLY)
////////////////////////// PWM INTERFACE FUNCTIONS /////////////////////////////

/**
  * @brief  This function will set the PWM to the required value
  * @param  port : the gpio port to use
  * @param  pin : the gpio pin to use
  * @param  clock_freq : frequency of the tim clock
  * @param  value : the value to push on the PWM output
  * @retval None
  */

// void pwm_start(PinName pin, uint32_t PWM_freq, uint32_t value, TimerCompareFormat_t resolution)
// {
//   TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM);
//   HardwareTimer *HT;
//   TimerModes_t previousMode;
//   uint32_t index = get_timer_index(Instance);
  
//   // If there is no timer for this index, create one
//   if (HardwareTimer_Handle[index] == NULL) {
//     static HardwareTimer timers[TIMER_NUM];
    
//     // We're using the no-args constructor and then calling setup manually
//     timers[index].setup(Instance);
    
//     // Now HardwareTimer_Handle[index] should be set
//   }
  
//   // At this point, HardwareTimer_Handle[index] should be set
//   HT = (HardwareTimer *)(HardwareTimer_Handle[index]->__this);
//   uint32_t channel = CH_PIN_CHANNEL(pinmap_function(pin, PinMap_TIM));
//   previousMode = HT->getMode(channel);
  
//   if (previousMode != TIMER_OUTPUT_COMPARE_PWM1) {
//     HT->setMode(channel, TIMER_OUTPUT_COMPARE_PWM1, pin);
//   }
  
//   HT->setOverflow(PWM_freq, HERTZ_FORMAT);
//   HT->setCaptureCompare(channel, value, resolution);
  
//   if (previousMode != TIMER_OUTPUT_COMPARE_PWM1) {
//     HT->resume();
//   }
// }

// void pwm_start(PinName pin, uint32_t PWM_freq, uint32_t value, TimerCompareFormat_t resolution)
// {  


//   TIM_OCInitTypeDef TIM_OCInitStructure = { 0 };
//   TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = { 0 };
//   uint32_t channel = getTimerChannel(pin);

//    // Configure pin if provided
//  if (pin != NC) {
//     pinmap_pinout(pin, PinMap_TIM);
// }
//   TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM);
// #if defined(TIM1_BASE)
//   if (Instance == TIM1) {
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
//   }
// #endif
// #if defined(TIM2_BASE)
//   if (Instance == TIM2) {
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//   }
// #endif
// #if defined(TIM3_BASE)
//   if (Instance == TIM3) {
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//   }
// #endif
// #if defined(TIM4_BASE)
//   if (Instance == TIM4) {
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
//   }
// #endif

//   // Get correct timer clock frequency
//   RCC_ClocksTypeDef RCC_ClocksStatus;
//   RCC_GetClocksFreq(&RCC_ClocksStatus);
//   uint32_t timerClock;

//   // APB1 peripherals get PCLK1, but if APB1 prescaler != 1, timer gets PCLK1*2
//   if (RCC_ClocksStatus.PCLK1_Frequency < RCC_ClocksStatus.SYSCLK_Frequency) {
//     timerClock = RCC_ClocksStatus.PCLK1_Frequency * 2;
//   } else {
//     timerClock = RCC_ClocksStatus.PCLK1_Frequency;
//   }

//   // Calculate prescaler and period for the target frequency
//   uint32_t prescaler = 1;
//   uint32_t period = timerClock / (PWM_freq * prescaler);

//   // Adjust prescaler if period is too large
//   while (period > 65535 && prescaler < 65535) {
//     prescaler++;
//     period = timerClock / (PWM_freq * prescaler);
//   }

//   // Ensure period isn't too small
//   if (period < 100) {
//     period = 100;
//     prescaler = timerClock / (PWM_freq * period);
//   }

//   // Configure Timer base
//   TIM_TimeBaseInitStructure.TIM_Period = period - 1;
//   TIM_TimeBaseInitStructure.TIM_Prescaler = prescaler - 1;
//   TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//   TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
//   TIM_TimeBaseInit(Instance, &TIM_TimeBaseInitStructure);

//   // Calculate pulse based on duty cycle
//   uint32_t maxValue = (1 << resolution) - 1;
//   uint32_t pulse = (period * value) / maxValue;


//   // Configure PWM for Channel 3
//   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//   TIM_OCInitStructure.TIM_Pulse = pulse;
//   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

//   if (Instance == TIM1 && channel != TIM_Channel_4) {
//     // TIM1 channels 1-3 have complementary outputs
//     TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
//     TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
//     TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
//     TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
//   }

//   // Initialize Instance Channel 3


//   if (Instance == TIM1) {
//     TIM_CtrlPWMOutputs(TIM1, ENABLE);
//   }

//   if (channel == TIM_Channel_1) {
//     // Initialize Instance Channel 1
//     TIM_OC1Init(Instance, &TIM_OCInitStructure);
//     TIM_OC1PreloadConfig(Instance, TIM_OCPreload_Enable);
//   } else if (channel == TIM_Channel_2) {
//     // Initialize Instance Channel 2
//     TIM_OC2Init(Instance, &TIM_OCInitStructure);
//     TIM_OC2PreloadConfig(Instance, TIM_OCPreload_Enable);
//   } else if (channel == TIM_Channel_3) {
//     // Initialize Instance Channel 3
//     TIM_OC3Init(Instance, &TIM_OCInitStructure);
//     TIM_OC3PreloadConfig(Instance, TIM_OCPreload_Enable);
//   } else if (channel == TIM_Channel_4) {
//     // Initialize Instance Channel 4
//     TIM_OC4Init(Instance, &TIM_OCInitStructure);
//     TIM_OC4PreloadConfig(Instance, TIM_OCPreload_Enable);
//   }
//   // Enable preload
//   TIM_ARRPreloadConfig(Instance, ENABLE);

//   // Enable the timer
//   TIM_Cmd(Instance, ENABLE);
// }

// *** WORKING *** //

// // Global structures to store timer configurations
// typedef struct {
//   uint32_t initialized;
//   uint32_t frequency;
//   uint32_t period;
// } TimerInfo_t;

// // Array to track timer configurations (for TIM1, TIM2, TIM3, TIM4)
// static TimerInfo_t timer_info[4] = {0};

// // Function to get timer index
// uint8_t getTimerIndex(TIM_TypeDef *Instance) {
// uint8_t idx = 0xFF; // Invalid index by default

// #if defined(TIM1_BASE)
// if (Instance == TIM1) idx = 0;
// #endif
// #if defined(TIM2_BASE)
// if (Instance == TIM2) idx = 1;
// #endif
// #if defined(TIM3_BASE)
// if (Instance == TIM3) idx = 2;
// #endif
// #if defined(TIM4_BASE)
// if (Instance == TIM4) idx = 3;
// #endif

// return idx;
// }

// // Function to only update PWM duty cycle without reinitializing the timer
// void pwm_update_duty(PinName pin, uint32_t value, TimerCompareFormat_t resolution) {
// uint32_t channel = getTimerChannel(pin);
// TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM);
// uint8_t timer_idx = getTimerIndex(Instance);

// if (timer_idx != 0xFF && timer_info[timer_idx].initialized) {
//   uint32_t period = timer_info[timer_idx].period;
//   uint32_t maxValue = (1 << resolution) - 1;
//   uint32_t pulse = (period * value) / maxValue;
  
//   // Update the duty cycle without changing timer configuration
//   switch (channel) {
//     case TIM_Channel_1:
//       TIM_SetCompare1(Instance, pulse);
//       break;
//     case TIM_Channel_2:
//       TIM_SetCompare2(Instance, pulse);
//       break;
//     case TIM_Channel_3:
//       TIM_SetCompare3(Instance, pulse);
//       break;
//     case TIM_Channel_4:
//       TIM_SetCompare4(Instance, pulse);
//       break;
//   }
// }
// }

// // Modified pwm_start function
// void pwm_start(PinName pin, uint32_t PWM_freq, uint32_t value, TimerCompareFormat_t resolution) {  
// TIM_OCInitTypeDef TIM_OCInitStructure = { 0 };
// TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = { 0 };
// uint32_t channel = getTimerChannel(pin);

// // Configure pin if provided
// if (pin != NC) {
//   pinmap_pinout(pin, PinMap_TIM);
// }

// TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM);
// uint8_t timer_idx = getTimerIndex(Instance);

// // Enable relevant timer clock
// #if defined(TIM1_BASE)
// if (Instance == TIM1) {
//   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
// }
// #endif
// #if defined(TIM2_BASE)
// if (Instance == TIM2) {
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
// }
// #endif
// #if defined(TIM3_BASE)
// if (Instance == TIM3) {
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
// }
// #endif
// #if defined(TIM4_BASE)
// if (Instance == TIM4) {
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
// }
// #endif

// // If timer is already initialized with the same frequency, just update duty cycle
// if (timer_idx != 0xFF && timer_info[timer_idx].initialized) {
//   if (timer_info[timer_idx].frequency == PWM_freq) {
//     pwm_update_duty(pin, value, resolution);
//     return;
//   }
// }

// // Get correct timer clock frequency
// RCC_ClocksTypeDef RCC_ClocksStatus;
// RCC_GetClocksFreq(&RCC_ClocksStatus);
// uint32_t timerClock;

// // Select proper clock source based on timer
// if (Instance == TIM1) {
//   // TIM1 is on APB2
//   if (RCC_ClocksStatus.PCLK2_Frequency < RCC_ClocksStatus.SYSCLK_Frequency) {
//     timerClock = RCC_ClocksStatus.PCLK2_Frequency * 2;
//   } else {
//     timerClock = RCC_ClocksStatus.PCLK2_Frequency;
//   }
// } else {
//   // TIM2, TIM3, TIM4 are on APB1
//   if (RCC_ClocksStatus.PCLK1_Frequency < RCC_ClocksStatus.SYSCLK_Frequency) {
//     timerClock = RCC_ClocksStatus.PCLK1_Frequency * 2;
//   } else {
//     timerClock = RCC_ClocksStatus.PCLK1_Frequency;
//   }
// }

// // Calculate prescaler and period for the target frequency
// uint32_t prescaler = 1;
// uint32_t period = timerClock / (PWM_freq * prescaler);

// // Adjust prescaler if period is too large
// while (period > 65535 && prescaler < 65535) {
//   prescaler++;
//   period = timerClock / (PWM_freq * prescaler);
// }

// // Ensure period isn't too small
// if (period < 100) {
//   period = 100;
//   prescaler = timerClock / (PWM_freq * period);
// }

// // Configure Timer base
// TIM_TimeBaseInitStructure.TIM_Period = period - 1;
// TIM_TimeBaseInitStructure.TIM_Prescaler = prescaler - 1;
// TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
// TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
// TIM_TimeBaseInit(Instance, &TIM_TimeBaseInitStructure);

// // Calculate pulse based on duty cycle
// uint32_t maxValue = (1 << resolution) - 1;
// uint32_t pulse = (period * value) / maxValue;

// // Configure PWM for the specified channel
// TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
// TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
// TIM_OCInitStructure.TIM_Pulse = pulse;
// TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

// if (Instance == TIM1 && channel != TIM_Channel_4) {
//   // TIM1 channels 1-3 have complementary outputs
//   TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
//   TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
//   TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
//   TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
// }

// // For TIM1, enable PWM outputs
// if (Instance == TIM1) {
//   TIM_CtrlPWMOutputs(TIM1, ENABLE);
// }

// // Initialize appropriate channel
// if (channel == TIM_Channel_1) {
//   TIM_OC1Init(Instance, &TIM_OCInitStructure);
//   TIM_OC1PreloadConfig(Instance, TIM_OCPreload_Enable);
// } else if (channel == TIM_Channel_2) {
//   TIM_OC2Init(Instance, &TIM_OCInitStructure);
//   TIM_OC2PreloadConfig(Instance, TIM_OCPreload_Enable);
// } else if (channel == TIM_Channel_3) {
//   TIM_OC3Init(Instance, &TIM_OCInitStructure);
//   TIM_OC3PreloadConfig(Instance, TIM_OCPreload_Enable);
// } else if (channel == TIM_Channel_4) {
//   TIM_OC4Init(Instance, &TIM_OCInitStructure);
//   TIM_OC4PreloadConfig(Instance, TIM_OCPreload_Enable);
// }

// // Enable preload
// TIM_ARRPreloadConfig(Instance, ENABLE);

// // Enable the timer
// TIM_Cmd(Instance, ENABLE);

// // Store timer information
// if (timer_idx != 0xFF) {
//   timer_info[timer_idx].initialized = 1;
//   timer_info[timer_idx].frequency = PWM_freq;
//   timer_info[timer_idx].period = period;
// }
// }

// Minimalist timer configuration
typedef struct {
  uint8_t flags;  // Bit 0-3: initialized flags
  uint16_t periods[4];
} TimerCfg_t;

static TimerCfg_t tcfg = {0};

// Function to update PWM duty cycle only
void pwm_update_duty(PinName pin, uint32_t value, TimerCompareFormat_t resolution) {
  uint32_t channel = getTimerChannel(pin);
  TIM_TypeDef *TIMx = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM);
  uint8_t idx = 0xFF;
  
  // Compact timer index lookup
  #if defined(TIM1_BASE)
  if (TIMx == TIM1) idx = 0;
  #endif
  #if defined(TIM2_BASE)
  if (TIMx == TIM2) idx = 1;
  #endif
  #if defined(TIM3_BASE)
  if (TIMx == TIM3) idx = 2;
  #endif
  #if defined(TIM4_BASE)
  if (TIMx == TIM4) idx = 3;
  #endif
  
  if (idx == 0xFF || !(tcfg.flags & (1 << idx))) return;
  
  // Calculate pulse
  uint32_t maxValue = (1UL << resolution) - 1;
  uint32_t pulse = (tcfg.periods[idx] * value) / maxValue;
  
  // Direct register access to update compare value
  switch (channel) {
    case TIM_Channel_1: TIMx->CH1CVR = pulse; break;
    case TIM_Channel_2: TIMx->CH2CVR = pulse; break;
    case TIM_Channel_3: TIMx->CH3CVR = pulse; break;
    case TIM_Channel_4: TIMx->CH4CVR = pulse; break;
  }
}

void pwm_start(PinName pin, uint32_t PWM_freq, uint32_t value, TimerCompareFormat_t resolution) {
  if (pin == NC) return;
  
  uint32_t channel = getTimerChannel(pin);
  pinmap_pinout(pin, PinMap_TIM);
  TIM_TypeDef *TIMx = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM);
  
  // Compact timer index lookup with clock enablement
  uint8_t idx = 0xFF;
  #if defined(TIM1_BASE)
  if (TIMx == TIM1) { 
    idx = 0; 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  }
  #endif
  #if defined(TIM2_BASE)
  if (TIMx == TIM2) { 
    idx = 1; 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  }
  #endif
  #if defined(TIM3_BASE)
  if (TIMx == TIM3) { 
    idx = 2; 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  }
  #endif
  #if defined(TIM4_BASE)
  if (TIMx == TIM4) { 
    idx = 3; 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  }
  #endif
  
  if (idx == 0xFF) return;
  
  // Check if already initialized
  if ((tcfg.flags & (1 << idx))) {
    static uint32_t last_freq = 0;
    if (last_freq == PWM_freq) {
      pwm_update_duty(pin, value, resolution);
      return;
    }
    last_freq = PWM_freq;
  }
  
  // Timer clock calculation
  RCC_ClocksTypeDef RCC_ClocksStatus;
  RCC_GetClocksFreq(&RCC_ClocksStatus);
  uint32_t timerClock = (TIMx == TIM1) ? 
      ((RCC_ClocksStatus.PCLK2_Frequency < RCC_ClocksStatus.SYSCLK_Frequency) ? 
      RCC_ClocksStatus.PCLK2_Frequency * 2 : RCC_ClocksStatus.PCLK2_Frequency) : 
      ((RCC_ClocksStatus.PCLK1_Frequency < RCC_ClocksStatus.SYSCLK_Frequency) ? 
      RCC_ClocksStatus.PCLK1_Frequency * 2 : RCC_ClocksStatus.PCLK1_Frequency);
  
  // Calculate prescaler and period
  uint16_t prescaler = 1;
  uint16_t period = timerClock / (PWM_freq * prescaler);
  
  while (period > 0xFFFF && prescaler < 0xFFFF) {
    prescaler++;
    period = timerClock / (PWM_freq * prescaler);
  }
  
  if (period < 100) {
    period = 100;
    prescaler = timerClock / (PWM_freq * period);
  }
  
  // Configure Timer base registers
  TIMx->ATRLR = period - 1;
  TIMx->PSC = prescaler - 1;
  TIMx->CTLR1 |= TIM_ARPE;  // Enable auto-reload preload
  
  // Calculate pulse width
  uint32_t maxValue = (1UL << resolution) - 1;
  uint32_t pulse = (period * value) / maxValue;
  
  // Working registers for channel setup
  uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;
  
  // Channel-specific setup
  switch (channel) {
    case TIM_Channel_1:
      // Disable channel
      TIMx->CCER &= ~TIM_CC1E;
      
      // Configure mode and output
      tmpccer = TIMx->CCER;
      tmpcr2 = TIMx->CTLR2;
      tmpccmrx = TIMx->CHCTLR1;
      
      // Clear mode and selection bits
      tmpccmrx &= ~TIM_OC1M;
      tmpccmrx &= ~TIM_CC1S;
      
      // Set PWM mode
      tmpccmrx |= TIM_OCMode_PWM1;
      
      // Set polarity and enable output
      tmpccer &= ~TIM_CC1P;
      tmpccer |= TIM_OCPolarity_High;
      tmpccer |= TIM_OutputState_Enable;
      
      // Setup for TIM1 complementary output
      if (TIMx == TIM1) {
        tmpccer &= ~TIM_CC1NP;
        tmpccer |= TIM_OCNPolarity_High;
        
        tmpccer &= ~TIM_CC1NE;
        tmpccer |= TIM_OutputNState_Enable;
        
        tmpcr2 &= ~TIM_OIS1;
        tmpcr2 &= ~TIM_OIS1N;
        
        tmpcr2 |= TIM_OCIdleState_Reset;
        tmpcr2 |= TIM_OCNIdleState_Reset;
      }
      
      // Write values to registers
      TIMx->CTLR2 = tmpcr2;
      TIMx->CHCTLR1 = tmpccmrx;
      TIMx->CH1CVR = pulse;
      TIMx->CCER = tmpccer;
      
      // Enable preload for this channel
      TIMx->CHCTLR1 |= TIM_OC1PE;
      break;
      
    case TIM_Channel_2:
      // Disable channel
      TIMx->CCER &= ~TIM_CC2E;
      
      // Configure mode and output
      tmpccer = TIMx->CCER;
      tmpcr2 = TIMx->CTLR2;
      tmpccmrx = TIMx->CHCTLR1;
      
      // Clear mode and selection bits
      tmpccmrx &= ~TIM_OC2M;
      tmpccmrx &= ~TIM_CC2S;
      
      // Set PWM mode (shifted for channel 2)
      tmpccmrx |= (TIM_OCMode_PWM1 << 8);
      
      // Set polarity and enable output
      tmpccer &= ~TIM_CC2P;
      tmpccer |= (TIM_OCPolarity_High << 4);
      tmpccer |= (TIM_OutputState_Enable << 4);
      
      // Setup for TIM1 complementary output
      if (TIMx == TIM1) {
        tmpccer &= ~TIM_CC2NP;
        tmpccer |= (TIM_OCNPolarity_High << 4);
        
        tmpccer &= ~TIM_CC2NE;
        tmpccer |= (TIM_OutputNState_Enable << 4);
        
        tmpcr2 &= ~TIM_OIS2;
        tmpcr2 &= ~TIM_OIS2N;
        
        tmpcr2 |= (TIM_OCIdleState_Reset << 2);
        tmpcr2 |= (TIM_OCNIdleState_Reset << 2);
      }
      
      // Write values to registers
      TIMx->CTLR2 = tmpcr2;
      TIMx->CHCTLR1 = tmpccmrx;
      TIMx->CH2CVR = pulse;
      TIMx->CCER = tmpccer;
      
      // Enable preload for this channel
      TIMx->CHCTLR1 |= TIM_OC2PE;
      break;
      
    case TIM_Channel_3:
      // Disable channel
      TIMx->CCER &= ~TIM_CC3E;
      
      // Configure mode and output
      tmpccer = TIMx->CCER;
      tmpcr2 = TIMx->CTLR2;
      tmpccmrx = TIMx->CHCTLR2;
      
      // Clear mode and selection bits
      tmpccmrx &= ~TIM_OC3M;
      tmpccmrx &= ~TIM_CC3S;
      
      // Set PWM mode
      tmpccmrx |= TIM_OCMode_PWM1;
      
      // Set polarity and enable output
      tmpccer &= ~TIM_CC3P;
      tmpccer |= (TIM_OCPolarity_High << 8);
      tmpccer |= (TIM_OutputState_Enable << 8);
      
      // Setup for TIM1 complementary output
      if (TIMx == TIM1) {
        tmpccer &= ~TIM_CC3NP;
        tmpccer |= (TIM_OCNPolarity_High << 8);
        
        tmpccer &= ~TIM_CC3NE;
        tmpccer |= (TIM_OutputNState_Enable << 8);
        
        tmpcr2 &= ~TIM_OIS3;
        tmpcr2 &= ~TIM_OIS3N;
        
        tmpcr2 |= (TIM_OCIdleState_Reset << 4);
        tmpcr2 |= (TIM_OCNIdleState_Reset << 4);
      }
      
      // Write values to registers
      TIMx->CTLR2 = tmpcr2;
      TIMx->CHCTLR2 = tmpccmrx;
      TIMx->CH3CVR = pulse;
      TIMx->CCER = tmpccer;
      
      // Enable preload for this channel
      TIMx->CHCTLR2 |= TIM_OC3PE;
      break;
      
    case TIM_Channel_4:
      // Disable channel
      TIMx->CCER &= ~TIM_CC4E;
      
      // Configure mode and output
      tmpccer = TIMx->CCER;
      tmpcr2 = TIMx->CTLR2;
      tmpccmrx = TIMx->CHCTLR2;
      
      // Clear mode and selection bits
      tmpccmrx &= ~TIM_OC4M;
      tmpccmrx &= ~TIM_CC4S;
      
      // Set PWM mode (shifted for channel 4)
      tmpccmrx |= (TIM_OCMode_PWM1 << 8);
      
      // Set polarity and enable output
      tmpccer &= ~TIM_CC4P;
      tmpccer |= (TIM_OCPolarity_High << 12);
      tmpccer |= (TIM_OutputState_Enable << 12);
      
      // Setup for TIM1 idle state
      if (TIMx == TIM1) {
        tmpcr2 &= ~TIM_OIS4;
        tmpcr2 |= (TIM_OCIdleState_Reset << 6);
      }
      
      // Write values to registers
      TIMx->CTLR2 = tmpcr2;
      TIMx->CHCTLR2 = tmpccmrx;
      TIMx->CH4CVR = pulse;
      TIMx->CCER = tmpccer;
      
      // Enable preload for this channel
      TIMx->CHCTLR2 |= TIM_OC4PE;
      break;
  }
  
  // For TIM1, enable main output
  if (TIMx == TIM1) {
    TIMx->BDTR |= TIM_MOE;
  }
  
  // Enable timer
  TIMx->CTLR1 |= TIM_CEN;
  
  // Store timer information
  tcfg.flags |= (1 << idx);
  tcfg.periods[idx] = period;
}

/**
  * @brief  This function will disable the PWM
  * @param  port : the gpio port to use
  * @param  pin : the gpio pin to use
  * @retval None
  */
// void pwm_stop(PinName pin)
// {
//   TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM);
//   HardwareTimer *HT;
//   uint32_t index = get_timer_index(Instance);
//   if (HardwareTimer_Handle[index] == NULL) {
//     HardwareTimer_Handle[index]->__this = new HardwareTimer((TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM));
//   }

//   HT = (HardwareTimer *)(HardwareTimer_Handle[index]->__this);
//   if (HT != NULL) {
//     delete (HT);
//     HT = NULL;
//   }
// }

void pwm_stop(PinName pin) {
  TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_TIM);
  if (Instance == NULL) {
      return; // Đảm bảo Instance hợp lệ
  }
  
  // Dừng timer trực tiếp thay vì dùng HardwareTimer
  Instance->CTLR1 &= ~TIM_CC1E; // Dừng bộ đếm
}

#endif /* TIM_MODULE_ENABLED && !TIM_MODULE_ONLY */

#ifdef __cplusplus
}
#endif


/* Tone.cpp

  A Tone Generator Library

  Written by Brett Hagman

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
//Modified 3 june 2024 by TempersLee for CH32duino

#include "Arduino.h"
#include "HardwareTimer.h"

#if defined(TIM_MODULE_ENABLED) && defined(TIMER_TONE) && !defined(TIM_MODULE_ONLY)

#define MAX_FREQ  65535

typedef struct {
  PinName pin;
  int32_t count;
} timerPinInfo_t;

static void timerTonePinInit(PinName p, uint32_t frequency, uint32_t duration);
static void tonePeriodElapsedCallback();
static timerPinInfo_t TimerTone_pinInfo = {NC, 0};
static HardwareTimer *TimerTone = NULL;

/**
  * @brief  Tone Period elapsed callback in non-blocking mode
  * @param  htim : timer handle
  * @retval None
  */
__attribute__((used)) __attribute__((noinline)) __attribute__((externally_visible))
static void tonePeriodElapsedCallback()
{
  GPIO_TypeDef *port = get_GPIO_Port(CH_PORT(TimerTone_pinInfo.pin));

  if (port != NULL) {
    if (TimerTone_pinInfo.count != 0) {
      if (TimerTone_pinInfo.count > 0) {
        TimerTone_pinInfo.count--;
      }
      digital_io_toggle(port, CH_MAP_GPIO_PIN(TimerTone_pinInfo.pin));
    } else {
      digital_io_write(port, CH_MAP_GPIO_PIN(TimerTone_pinInfo.pin), 0);
    }
  }
}

/**
  * @brief  This function will reset the tone timer
  * @param  port : pointer to port
  * @param  pin : pin number to toggle
  * @retval None
  */
__attribute__((used)) __attribute__((noinline)) __attribute__((externally_visible))
static void timerTonePinDeinit()
{
  if (TimerTone != NULL) {
    TimerTone->timerHandleDeinit();
  }
  if (TimerTone_pinInfo.pin != NC) {
    // Ensure pin is LOW before changing to input mode
    GPIO_TypeDef *port = get_GPIO_Port(CH_PORT(TimerTone_pinInfo.pin));
    if (port != NULL) {
      digital_io_write(port, CH_MAP_GPIO_PIN(TimerTone_pinInfo.pin), 0);
    }
    // pin_function(TimerTone_pinInfo.pin, CH_PIN_DATA(CH_MODE_INPUT, CH_CNF_INPUT_FLOAT, NOPULL, AFIO_NONE));
    TimerTone_pinInfo.pin = NC;
  }
}

__attribute__((used)) __attribute__((noinline)) __attribute__((externally_visible))
static void timerTonePinInit(PinName p, uint32_t frequency, uint32_t duration)
{
  uint32_t timFreq = 2 * frequency;

  if (frequency <= MAX_FREQ) {
    if (frequency == 0) {
      if (TimerTone != NULL) {
        TimerTone->pause();
      }
    } else {
      TimerTone_pinInfo.pin = p;

      //Calculate the toggle count
      if (duration > 0) {
        TimerTone_pinInfo.count = ((timFreq * duration) / 1000);
      } else {
        TimerTone_pinInfo.count = -1;
      }

      pin_function(TimerTone_pinInfo.pin, CH_PIN_DATA(CH_MODE_OUTPUT_50MHz, CH_CNF_OUTPUT_PP, NOPULL, AFIO_NONE));

      TimerTone->setOverflow(timFreq, HERTZ_FORMAT);
      TimerTone->attachInterrupt(tonePeriodElapsedCallback);
      TimerTone->resume();
    }
  }
}
__attribute__((used)) __attribute__((noinline)) __attribute__((externally_visible))
// frequency (in hertz) and duration (in milliseconds).
void tone(uint8_t _pin, unsigned int frequency, unsigned long duration)
{
  PinName p = digitalPinToPinName(_pin);

  if (TimerTone == NULL) {
    TimerTone = new HardwareTimer(TIMER_TONE);
  }

  if (p != NC) {
    if ((TimerTone_pinInfo.pin == NC) || (TimerTone_pinInfo.pin == p)) {
      timerTonePinInit(p, frequency, duration);
    }
  }
}
__attribute__((used)) __attribute__((noinline)) __attribute__((externally_visible))
void noTone(uint8_t _pin, bool destruct)
{
  PinName p = digitalPinToPinName(_pin);
  if ((p != NC) && (TimerTone_pinInfo.pin == p) && (TimerTone != NULL)) {
    if (destruct) {
      timerTonePinDeinit();
      delete (TimerTone);
      TimerTone = NULL;
    } else {
      TimerTone->pause();
    }
  }
}
#else
#warning "TIMER_TONE or TIM_MODULE_ENABLED not defined"
__attribute__((used)) __attribute__((noinline)) __attribute__((externally_visible))
void tone(uint8_t _pin, unsigned int frequency, unsigned long duration)
{
  (void)(_pin);
  (void)(frequency);
  (void)(duration);
}

__attribute__((used)) __attribute__((noinline)) __attribute__((externally_visible))
void noTone(uint8_t _pin, bool destruct)
{
  (void)(_pin);
  (void)(destruct);
}
#endif /* TIM_MODULE_ENABLED && TIMER_TONE && !TIM_MODULE_ONLY*/

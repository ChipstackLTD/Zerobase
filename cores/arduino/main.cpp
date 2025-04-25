#define ARDUINO_MAIN

#include "Arduino.h"
#include "debug.h"

/*
 * \brief Main entry point of Arduino application
 */

uint32_t z1 = 0;
uint32_t z2 = 0;
 
int main(void)
{
    pre_init();

#ifdef BOARD_ZEROBASE
    if (FLASH->STATR & (1 << 14))
        NVIC_SystemReset();
    SystemReset_StartMode(Start_Mode_BOOT);
    // pinMode(PD4, OUTPUT);
      // Bật clock cho GPIOD (PDx)
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;

      // Xóa 4 bit cấu hình của pin PD4 (pin 12)
      // PD4 là pin thứ 4 -> vị trí = 4 * 4 = 16 trong CFGLR
    GPIOD->CFGLR &= ~(0xF << (4 * 4));     // Xóa cấu hình cũ
  
      // Thiết lập: Output push-pull, tốc độ 50MHz -> 0b0011 = 0x3
    GPIOD->CFGLR |= (0x3 << (4 * 4));      // Đặt cấu hình mới
  
#endif

#if defined(USE_TINYUSB)
    if (TinyUSB_Device_Init)
    {
        TinyUSB_Device_Init(0);
    }
#endif

uint16_t c = 0xFFFF;

for (size_t i = 0; i < 8; i++) {
    uint8_t byte = (i < 4) ? (((*(uint32_t *)0x1FFFF7E8) >> (24 - (i * 8))) & 0xFF) 
                           : (((*(uint32_t *)0x1FFFF7EC) >> (24 - ((i - 4) * 8))) & 0xFF);
    c ^= byte;
    for (int j = 0; j < 8; j++) {
        c = (c & 0x0001) ? ((c >> 1) ^ 0xA001) : (c >> 1);
    }
}

if ((uint16_t)(*(__IO uint16_t *)0x1FFFF806) != (uint16_t)(((~((c >> 8) & 0xFF) << 8) | ((c >> 8) & 0xFF))) &&
    (uint16_t)(*(__IO uint16_t *)0x1FFFF804) != (uint16_t)(((~(c & 0xFF) << 8) | (c & 0xFF)))) {
    *(volatile uint32_t*)0xE000E048 = 0xBEEF0000 | (1 << 7);
}

  setup();

    do
    {
        loop();
#if defined(USE_TINYUSB)
        if (TinyUSB_Device_Task)
        {
            TinyUSB_Device_Task();
        }
        if (TinyUSB_Device_FlushCDC)
        {
            TinyUSB_Device_FlushCDC();
        }
#endif
    } while (1);

    return 0;
}

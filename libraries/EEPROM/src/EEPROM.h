/*
  EEPROM.h - Enhanced EEPROM library for CH32V microcontrollers
  Supporting both CH32V20x (BOARD_ZEROBASE2) and CH32V003 (BOARD_ZEROBASE)
*/

#ifndef EEPROM_h
#define EEPROM_h
#include <Arduino.h>

// Board-specific flash memory definitions
#ifdef BOARD_ZEROBASE2
  // 256-byte page configuration for BOARD_ZEROBASE2
  #define EEPROM_FLASH_PAGE_SIZE     256     // 256 bytes per page
  #define EEPROM_FLASH_BASE_ADDR     ((uint32_t)0x08037F00) // Last 256-byte page in 224KB flash
  #define EEPROM_SIZE                EEPROM_FLASH_PAGE_SIZE // Full page size for EEPROM emulation
#elif defined(BOARD_ZEROBASE)
  // 64-byte page configuration for BOARD_ZEROBASE
  #define EEPROM_FLASH_PAGE_SIZE     64      // 64 bytes per page
  #define EEPROM_FLASH_BASE_ADDR     ((uint32_t)0x08003FC0) // Last 64-byte page in 16KB flash
  #define EEPROM_SIZE                EEPROM_FLASH_PAGE_SIZE // Full page size for EEPROM emulation
#else
  // Default configuration (use smallest size to be safe)
  #define EEPROM_FLASH_PAGE_SIZE     64      // 64 bytes per page
  #define EEPROM_FLASH_BASE_ADDR     ((uint32_t)0x08003FC0) // Assume last 64-byte page
  #define EEPROM_SIZE                EEPROM_FLASH_PAGE_SIZE
#endif

// Include the appropriate flash header based on the board
#ifdef BOARD_ZEROBASE
  #include <ch32v00x_flash.h>
#else
  #include <ch32v20x_flash.h>
#endif

class EEPROMClass {
  public:
    EEPROMClass(void);
    ~EEPROMClass(void);

    void begin(size_t size = EEPROM_SIZE);

    uint8_t * getDataPtr();
    uint8_t const * getConstDataPtr() const;

    uint8_t read(int const idx);
    void write(int const idx, uint8_t const val);   // requires commit() to make data stick
    void erase(void);                               // requires commit() to make data stick

    bool commit(void);
    bool end(void);

    template<typename T> 
    T &get(int const address, T &t) {
      if (address < 0 || address + sizeof(T) > _size)
        return t;
      memcpy((uint8_t*) &t, _data + address, sizeof(T));
      return t;
    }

    template<typename T> 
    const T &put(int const address, const T &t) {
      if (address < 0 || address + sizeof(T) > _size)
        return t;
      if (memcmp(_data + address, (const uint8_t*)&t, sizeof(T)) != 0) {
        _dirty = true;
        memcpy(_data + address, (const uint8_t*)&t, sizeof(T));
      }
      return t;
    }

    size_t length() { return _size; }

    uint8_t& operator[](int const address) { return getDataPtr()[address]; }
    uint8_t const & operator[](int const address) const { return getConstDataPtr()[address]; }

  protected:
    uint8_t* _data = nullptr;
    bool _dirty = false;
    size_t _size = 0;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
extern EEPROMClass EEPROM;
#endif

#endif // #ifndef EEPROM_h
/*
  EEPROM.cpp - Enhanced EEPROM library for CH32V microcontrollers
  Supporting both CH32V20x (BOARD_ZEROBASE2) and CH32V003 (BOARD_ZEROBASE)
*/

#include "EEPROM.h"

EEPROMClass::EEPROMClass(void) {
}

EEPROMClass::~EEPROMClass() {
  end();
}

void EEPROMClass::begin(size_t size) {
  if (size > EEPROM_SIZE) {
    size = EEPROM_SIZE; // Limit to flash page size
  }
  _size = size;

  // Allocate data buffer and copy the current content from flash
  if (!_data) {
    // Allocate the RAM data buffer
    _data = new uint8_t[_size];
    
    // Copy current data from flash storage
    for (size_t i = 0; i < _size; i++) {
      _data[i] = *((uint8_t*)(EEPROM_FLASH_BASE_ADDR + i));
    }
  }
  
  _dirty = false;
}

uint8_t * EEPROMClass::getDataPtr() {
  _dirty = true;
  return &_data[0];
}

uint8_t const * EEPROMClass::getConstDataPtr() const {
  return &_data[0];
}

uint8_t EEPROMClass::read(int const idx) {
  if (_data && idx >= 0 && (size_t)idx < _size) {
    return _data[idx];
  }
  return 0;
}

void EEPROMClass::write(int const idx, uint8_t const val) {
  if (_data && idx >= 0 && (size_t)idx < _size) {
    if (_data[idx] != val) {
      _dirty = true;
      _data[idx] = val;
    }
  }
}

void EEPROMClass::erase(void) {
  if (_data) {
    _dirty = true;
    for (size_t i = 0; i < _size; i++) {
      _data[i] = 0xFF;
    }
  }
}

bool EEPROMClass::commit() {
  if (!_dirty || !_data) {
    return true; // Nothing to do
  }

  // Unlock Flash
  FLASH_Unlock();
  
  #ifdef BOARD_ZEROBASE2
  // For BOARD_ZEROBASE2 with FLASH_ROM_ERASE/FLASH_ROM_WRITE functions
  
  // Configure flash access clock if system clock > 120MHz
  // FLASH_Access_Clock_Cfg(FLASH_Access_SYSTEM_HALF); // Uncomment if needed
  
  // Unlock Fast Programming Mode
  FLASH_Unlock_Fast();
  
  // Erase the page
  FLASH_Status status = FLASH_ROM_ERASE(EEPROM_FLASH_BASE_ADDR, EEPROM_FLASH_PAGE_SIZE);
  if (status != FLASH_COMPLETE) {
    FLASH_Lock_Fast();
    FLASH_Lock();
    return false;
  }
  
  // Create aligned buffer for writing
  uint32_t aligned_data[EEPROM_FLASH_PAGE_SIZE / 4];
  
  // Convert byte array to word array for proper flash writing
  for (size_t i = 0; i < EEPROM_FLASH_PAGE_SIZE / 4; i++) {
    aligned_data[i] = 0;
    for (int j = 0; j < 4 && (i*4+j) < _size; j++) {
      aligned_data[i] |= ((uint32_t)_data[i*4+j]) << (j*8);
    }
  }
  
  // Write the data
  status = FLASH_ROM_WRITE(EEPROM_FLASH_BASE_ADDR, aligned_data, EEPROM_FLASH_PAGE_SIZE);
  
  // Lock Flash again
  FLASH_Lock_Fast();
  FLASH_Lock();
  
  if (status != FLASH_COMPLETE) {
    return false;
  }
  
  #elif defined(BOARD_ZEROBASE)
  // For BOARD_ZEROBASE with CH32V003 using direct page operations
  
  // Unlock Fast Programming Mode
  FLASH_Unlock_Fast();
  
  // Erase the page
  FLASH_ErasePage_Fast(EEPROM_FLASH_BASE_ADDR);
  
  // For CH32V003, the FLASH_ProgramPage_Fast function doesn't take a data buffer parameter
  // We need to use FLASH_BufLoad to load each word and then call FLASH_ProgramPage_Fast
  
  // Reset the buffer
  FLASH_BufReset();
  
  // Load data into the buffer
  for (size_t i = 0; i < _size; i += 4) {
    uint32_t data_word = 0;
    for (int j = 0; j < 4 && (i+j) < _size; j++) {
      data_word |= ((uint32_t)_data[i+j]) << (j*8);
    }
    
    // Load word into buffer
    FLASH_BufLoad(EEPROM_FLASH_BASE_ADDR + i, data_word);
  }
  
  // Program the page
  FLASH_ProgramPage_Fast(EEPROM_FLASH_BASE_ADDR);
  
  // Lock Flash again
  FLASH_Lock_Fast();
  #else
  // Generic approach using standard FLASH functions
  
  // Erase the page
  FLASH_ErasePage(EEPROM_FLASH_BASE_ADDR);
  
  // Program the data word by word
  for (size_t i = 0; i < _size; i += 4) {
    uint32_t data_word = 0;
    for (int j = 0; j < 4 && (i+j) < _size; j++) {
      data_word |= ((uint32_t)_data[i+j]) << (j*8);
    }
    
    if (FLASH_ProgramWord(EEPROM_FLASH_BASE_ADDR + i, data_word) != FLASH_COMPLETE) {
      FLASH_Lock();
      return false;
    }
  }
  #endif
  
  FLASH_Lock();
  
  _dirty = false;
  return true;
}

bool EEPROMClass::end() {
  bool retval = commit();
  
  if (_data) {
    delete[] _data;
    _data = nullptr;
  }
  
  _size = 0;
  return retval;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
EEPROMClass EEPROM;
#endif
/*
 * ZBEeprom24Cxx.cpp - Library implementation for 24Cxx series I2C EEPROM chips
 * Created by Claude, May 3, 2025
 */

 #include "ZBEeprom24Cxx.h"
 #include <string.h>  // For strlen
 
 // Constructor
 ZBEeprom24Cxx::ZBEeprom24Cxx(long deviceSize, byte deviceAddress) {
   _deviceSize = deviceSize;
   _deviceAddress = deviceAddress;
   calculateDeviceParameters();
 }
 
 // Initialize with selected Wire interface
 void ZBEeprom24Cxx::begin(TwoWire &wirePort, bool initWire) {
   _wire = &wirePort;
   
   // Initialize the Wire interface if requested
   if (initWire) {
     _wire->begin();
   }
 }
 
 // Calculate page size and address size based on device size
 void ZBEeprom24Cxx::calculateDeviceParameters(void) {
   // Set address size (1 byte for small EEPROMs, 2 bytes for larger ones)
   if (_deviceSize <= AT24C16) {
     _addressSize = 1;
   } else {
     _addressSize = 2;
   }
   
   // Set page size based on EEPROM model
   if (_deviceSize <= AT24C16) {
     _pageSize = 16;  // 16 bytes per page for smaller EEPROMs
   } else {
     _pageSize = 64;  // 64 bytes per page for larger EEPROMs
   }
 }
 
 // Custom min function
 int ZBEeprom24Cxx::_min(int a, int b) {
   return (a < b) ? a : b;
 }
 
 // Get high byte of address
 byte ZBEeprom24Cxx::getAddressHighByte(int address) {
   return (address >> 8) & 0xFF;
 }
 
 // Get low byte of address
 byte ZBEeprom24Cxx::getAddressLowByte(int address) {
   return address & 0xFF;
 }
 
 // Write address bytes to I2C bus
 void ZBEeprom24Cxx::writeAddressBytes(int address) {
   if (_addressSize == 2) {
     _wire->write(getAddressHighByte(address));
   }
   _wire->write(getAddressLowByte(address));
 }
 
 // Check if the EEPROM is responding
 bool ZBEeprom24Cxx::isConnected(void) {
   _wire->beginTransmission(_deviceAddress);
   return (_wire->endTransmission() == 0);
 }
 
 // Wait for EEPROM to be ready (ACK polling)
 void ZBEeprom24Cxx::waitForReady(void) {
   while (!isConnected()) {
     delay(1);  // Wait until device responds
   }
 }
 
 // Write a single byte
 bool ZBEeprom24Cxx::write_1_byte(int address, byte data) {
   if (address >= _deviceSize) {
     return false;  // Address out of range
   }
   
   _wire->beginTransmission(_deviceAddress);
   writeAddressBytes(address);
   _wire->write(data);
   bool result = (_wire->endTransmission() == 0);
   
   // Wait for the write cycle to complete
   waitForReady();
   
   return result;
 }
 
 // Read a single byte
 byte ZBEeprom24Cxx::read_1_byte(int address) {
   if (address >= _deviceSize) {
     return 0;  // Address out of range
   }
   
   // Set the address pointer
   _wire->beginTransmission(_deviceAddress);
   writeAddressBytes(address);
   if (_wire->endTransmission() != 0) {
     return 0;
   }
   
   // Request one byte
   if (_wire->requestFrom(_deviceAddress, (byte)1) != 1) {
     return 0;
   }
   
   if (_wire->available()) {
     return _wire->read();
   }
   
   return 0;
 }
 
 // Write a 2-byte value
 bool ZBEeprom24Cxx::write_2_byte(int address, int data) {
   if (address + 2 > _deviceSize) {
     return false;  // Address out of range
   }
   
   byte buffer[2];
   buffer[0] = data & 0xFF;           // Low byte
   buffer[1] = (data >> 8) & 0xFF;    // High byte
   
   return writeBytes(address, buffer, 2);
 }
 
 // Read a 2-byte value
 int ZBEeprom24Cxx::read_2_byte(int address) {
   if (address + 2 > _deviceSize) {
     return 0;  // Address out of range
   }
   
   byte buffer[2];
   if (readBytes(address, buffer, 2) != 2) {
     return 0;
   }
   
   return (int)buffer[0] | ((int)buffer[1] << 8);
 }
 
 // Write a 4-byte value
 bool ZBEeprom24Cxx::write_4_byte(int address, long data) {
   if (address + 4 > _deviceSize) {
     return false;  // Address out of range
   }
   
   byte buffer[4];
   buffer[0] = data & 0xFF;           // Byte 0 (LSB)
   buffer[1] = (data >> 8) & 0xFF;    // Byte 1
   buffer[2] = (data >> 16) & 0xFF;   // Byte 2
   buffer[3] = (data >> 24) & 0xFF;   // Byte 3 (MSB)
   
   return writeBytes(address, buffer, 4);
 }
 
 // Read a 4-byte value
 long ZBEeprom24Cxx::read_4_byte(int address) {
   if (address + 4 > _deviceSize) {
     return 0;  // Address out of range
   }
   
   byte buffer[4];
   if (readBytes(address, buffer, 4) != 4) {
     return 0;
   }
   
   return (long)buffer[0] | 
          ((long)buffer[1] << 8) | 
          ((long)buffer[2] << 16) | 
          ((long)buffer[3] << 24);
 }
 
 // Write multiple bytes (respecting page boundaries)
 bool ZBEeprom24Cxx::writeBytes(int address, byte *data, int length) {
   if (address + length > _deviceSize) {
     return false;  // Would exceed EEPROM size
   }
   
   int bytesWritten = 0;
   
   while (bytesWritten < length) {
     // Calculate current page and bytes remaining in this page
     int currentPage = (address + bytesWritten) / _pageSize;
     int pageEndAddress = (currentPage + 1) * _pageSize - 1;
     int bytesRemainInPage = pageEndAddress - (address + bytesWritten) + 1;
     
     // Determine how many bytes to write in this iteration
     int bytesToWrite = _min(bytesRemainInPage, (length - bytesWritten));
     
     // Write the bytes for this page
     _wire->beginTransmission(_deviceAddress);
     writeAddressBytes(address + bytesWritten);
     
     for (int i = 0; i < bytesToWrite; i++) {
       _wire->write(data[bytesWritten + i]);
     }
     
     if (_wire->endTransmission() != 0) {
       return false;
     }
     
     // Wait for the write cycle to complete
     waitForReady();
     
     // Update counters
     bytesWritten += bytesToWrite;
   }
   
   return true;
 }
 
 // Read multiple bytes
 int ZBEeprom24Cxx::readBytes(int address, byte *data, int length) {
   if (address + length > _deviceSize) {
     return 0;  // Would exceed EEPROM size
   }
   
   // Set the address pointer
   _wire->beginTransmission(_deviceAddress);
   writeAddressBytes(address);
   if (_wire->endTransmission() != 0) {
     return 0;
   }
   
   // For some devices, we might need to read in smaller chunks
   int maxReadSize = 32;  // Maximum I2C buffer size for many Arduino boards
   int bytesRead = 0;
   
   while (bytesRead < length) {
     int bytesToRead = _min(maxReadSize, (length - bytesRead));
     
     int available = _wire->requestFrom(_deviceAddress, (byte)bytesToRead);
     if (available != bytesToRead) {
       return bytesRead;  // Return bytes read so far
     }
     
     for (int i = 0; i < bytesToRead; i++) {
       if (_wire->available()) {
         data[bytesRead + i] = _wire->read();
       } else {
         return bytesRead + i;  // Return bytes read so far
       }
     }
     
     bytesRead += bytesToRead;
     
     // If we have more to read, we need to set the address pointer again
     if (bytesRead < length) {
       _wire->beginTransmission(_deviceAddress);
       writeAddressBytes(address + bytesRead);
       if (_wire->endTransmission() != 0) {
         return bytesRead;  // Return bytes read so far
       }
     }
   }
   
   return bytesRead;  // Return the number of bytes read
 }
 
 // Write a string (including null terminator) using Arduino String class
 bool ZBEeprom24Cxx::writeString(int address, const String &str) {
   int length = str.length() + 1;  // +1 for null terminator
   return writeBytes(address, (byte*)str.c_str(), length);
 }
 
 // Read a string as Arduino String class
 String ZBEeprom24Cxx::readString(int address, int maxLength) {
   char buffer[maxLength];
   
   // Set the address pointer
   _wire->beginTransmission(_deviceAddress);
   writeAddressBytes(address);
   if (_wire->endTransmission() != 0) {
     return String("");  // Empty string on error
   }
   
   // Determine how many bytes to read (up to null terminator or maxLength)
   int bytesToRead = maxLength - 1;  // Leave room for null terminator
   int bytesRead = 0;
   bool nullFound = false;
   
   while (bytesRead < bytesToRead) {
     int blockSize = _min(32, bytesToRead - bytesRead);  // Read in blocks of 32 bytes max
     
     int available = _wire->requestFrom(_deviceAddress, (byte)blockSize);
     if (available == 0) {
       break;  // No more bytes available
     }
     
     for (int i = 0; i < available; i++) {
       if (_wire->available()) {
         byte c = _wire->read();
         buffer[bytesRead++] = c;
         
         if (c == 0) {  // Found null terminator
           nullFound = true;
           break;
         }
       } else {
         break;
       }
     }
     
     if (nullFound) {
       break;
     }
     
     // If we need to read more and didn't find a null terminator
     if (bytesRead < bytesToRead && !nullFound) {
       _wire->beginTransmission(_deviceAddress);
       writeAddressBytes(address + bytesRead);
       if (_wire->endTransmission() != 0) {
         break;
       }
     }
   }
   
   // Ensure null termination
   buffer[bytesRead] = 0;
   
   // Create a String object from the buffer
   return String(buffer);
 }
 
 // Legacy C-string write function
 bool ZBEeprom24Cxx::writeString(int address, const char *data) {
   int length = strlen(data) + 1;  // +1 for null terminator
   return writeBytes(address, (byte*)data, length);
 }
 
 // Legacy C-string read function
 bool ZBEeprom24Cxx::readString(int address, char *data, int maxLength) {
   int bytesRead = readBytes(address, (byte*)data, maxLength - 1);
   
   // Ensure null termination
   data[bytesRead < maxLength - 1 ? bytesRead : maxLength - 1] = '\0';
   
   return bytesRead > 0;
 }
 
 // Clear entire EEPROM with specified value
 bool ZBEeprom24Cxx::clear(byte clearValue) {
   return clearRange(0, _deviceSize, clearValue);
 }
 
 // Clear a range of EEPROM with specified value
 bool ZBEeprom24Cxx::clearRange(int startAddress, int length, byte clearValue) {
   if (startAddress + length > _deviceSize) {
     return false;  // Would exceed EEPROM size
   }
   
   // Create a buffer with the clear value (use a smaller buffer to save RAM)
   byte buffer[16];  // Reduced from _pageSize to save memory
   for (byte i = 0; i < sizeof(buffer); i++) {
     buffer[i] = clearValue;
   }
   
   int bytesCleared = 0;
   
   while (bytesCleared < length) {
     int bytesToClear = _min((int)sizeof(buffer), (length - bytesCleared));
     
     if (!writeBytes(startAddress + bytesCleared, buffer, bytesToClear)) {
       return false;
     }
     
     bytesCleared += bytesToClear;
   }
   
   return true;
 }
 
 // Return the EEPROM size
 long ZBEeprom24Cxx::length(void) {
   return _deviceSize;
 }
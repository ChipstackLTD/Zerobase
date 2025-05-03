 #ifndef ZBEeprom24Cxx_h
 #define ZBEeprom24Cxx_h
 
 #include "Arduino.h"
 #include <Wire.h>
 
 // Define different EEPROM models (sizes in bytes)
 #define AT24C01  128    // 16 x 8
 #define AT24C02  256    // 32 x 8
 #define AT24C04  512    // 64 x 8
 #define AT24C08  1024   // 128 x 8
 #define AT24C16  2048   // 256 x 8
 #define AT24C32  4096   // 512 x 8
 #define AT24C64  8192   // 1024 x 8
 #define AT24C128 16384  // 2048 x 8
 #define AT24C256 32768  // 4096 x 8
 #define AT24C512 65536  // 8192 x 8
 
 class ZBEeprom24Cxx {
   public:
     // Constructor with optional device address (default is 0x50)
     ZBEeprom24Cxx(long deviceSize = AT24C256, byte deviceAddress = 0x50);
     
     // Initialize the library and I2C interface
     void begin(TwoWire &wirePort = Wire, bool initWire = true);
     
     // Basic functions
     bool write_1_byte(int address, byte data);
     byte read_1_byte(int address);
     
     // Multi-byte operations
     bool write_2_byte(int address, int data);
     int read_2_byte(int address);
     bool write_4_byte(int address, long data);
     long read_4_byte(int address);
     
     // Multiple byte operations
     bool writeBytes(int address, byte *data, int length);
     int readBytes(int address, byte *data, int length);
     
     // String operations with Arduino String class
     bool writeString(int address, const String &str);
     String readString(int address, int maxLength = 100);
     
     // Legacy string operations with C-strings
     bool writeString(int address, const char *data);
     bool readString(int address, char *data, int maxLength);
     
     // Utility functions
     bool clear(byte clearValue = 0xFF);
     bool clearRange(int startAddress, int length, byte clearValue = 0xFF);
     long length(void); // Returns the EEPROM size
     
     // Wait for EEPROM to be ready
     void waitForReady(void);
     
     // Check if the EEPROM is responding
     bool isConnected(void);
     
   private:
     TwoWire *_wire;           // Wire instance (Wire or Wire1)
     byte _deviceAddress;      // I2C device address
     long _deviceSize;         // EEPROM size in bytes
     byte _pageSize;           // Page size for page write operations
     byte _addressSize;        // 1 or 2 bytes for address
 
     // Calculate page size and address size based on device size
     void calculateDeviceParameters(void);
     
     // Get high byte of address
     byte getAddressHighByte(int address);
     
     // Get low byte of address
     byte getAddressLowByte(int address);
     
     // Write address bytes to I2C bus
     void writeAddressBytes(int address);
     
     // Minimum function to avoid conflicts
     int _min(int a, int b);
 };
 
 #endif // ZBEeprom24Cxx_h
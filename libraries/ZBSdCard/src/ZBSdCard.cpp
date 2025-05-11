/**
 * ZBSdCard.cpp - Highly optimized library for SD card operations on CH32V003/CH32V203
 * Dual-platform support with minimized RAM and flash footprint
 */

#ifdef BOARD_ZEROBASE2
#include "ZBSdCard.h"
 // Constructor
 ZBSdCard::ZBSdCard() {
  _spi = nullptr;
  _isSDHC = false;
  _partitionStartSector = 0;
  _fatStartSector = 0;
  _rootDirCluster = 0;
  _dataStartSector = 0;
  _sectorsPerCluster = 0;
  _clusterSize = 0;
  _firstFreeFAT = 2;  // Start from cluster 2 (first valid data cluster)
  _lastError = SD_SUCCESS;
}
 
  // Modified begin function to accept SPI interface
  bool ZBSdCard::begin(uint8_t csPin, SPIClass& spi) {
    _csPin = csPin;
    _spi = &spi;  // Store the SPI interface pointer
    _lastError = SD_SUCCESS;
    
    // Initialize SPI
    _spi->begin();
    _spi->setDataMode(SPI_MODE0);
    #if defined(BOARD_ZEROBASE)
    _spi->setClockDivider(SPI_CLOCK_DIV32);  // Slow speed for init
    #endif
    #if defined(BOARD_ZEROBASE2)
    _spi->setClockDivider(SPI_CLOCK_DIV128);  // Slow speed for init
    #endif
    
    // Set chip select pin as output
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
    
    // Add delay for SD card power-up
    delay(100);
    
    // Extra clock cycles with CS high to ensure card is ready
    for (int i = 0; i < 10; i++) {
      _spi->transfer(0xFF);
    }
    
    // Initialize SD card
    if (!initializeSD()) {
      _lastError = SD_ERROR_INIT;
      return false;
    }
    
    // Find the partition and read FAT32 boot sector
    if (!findPartition()) {
      _lastError = SD_ERROR_DISK_READ;
      return false;
    }
    
    if (!readFAT32BootSector()) {
      _lastError = SD_ERROR_DISK_READ;
      return false;
    }
    
    #if defined(BOARD_ZEROBASE)
    _spi->setClockDivider(SPI_CLOCK_DIV16);  // Conservative speed for data transfer
    #endif
    #if defined(BOARD_ZEROBASE2)
    // Now set to the requested SPI speed for data transfer
    _spi->setClockDivider(SPI_CLOCK_DIV64);  // Conservative speed for data transfer
    #endif
    return true;
  }
 
 // Function to initialize the SD card
 bool ZBSdCard::initializeSD() {
   uint8_t response;
   uint16_t retry = 0;
 
   digitalWrite(_csPin, HIGH);
 
   // Send at least 74 clock pulses with CS high (10 bytes = 80 clocks)
   for (int i = 0; i < 10; i++) {
     _spi->transfer(0xFF);
   }
 
   // Put SD card in idle state (CMD0)
   response = sendCommand(SD_CMD0, 0);
   if (response != SD_R1_IDLE_STATE) {
     return false;
   }
 
   // Send interface condition command (CMD8)
   response = sendCommand(SD_CMD8, 0x1AA);  // 0x1AA = 3.3V and check pattern
 
   if (response & SD_R1_ILLEGAL_CMD) {
     // Card is probably SDv1 or MMC
     return false;
   } else {
     // Card is SDv2
     // Check voltage range and check pattern
     uint32_t r7 = _spi->transfer(0xFF) << 24;
     r7 |= _spi->transfer(0xFF) << 16;
     r7 |= _spi->transfer(0xFF) << 8;
     r7 |= _spi->transfer(0xFF);
 
     if ((r7 & 0x1FF) != 0x1AA) {
       return false;
     }
   }
 
   // Initialize card (ACMD41 with HCS bit)
   retry = 0;
   do {
     response = sendAppCommand(SD_ACMD41, 0x40000000);  // HCS bit set
 
     if (response & SD_R1_ILLEGAL_CMD) {
       return false;
     }
 
     retry++;
     if (retry >= 1000) {
       return false;
     }
 
     // Short delay between attempts
     delay(10);
 
   } while (response != SD_R1_READY_STATE);
 
   // Check card capacity (CMD58)
   response = sendCommand(SD_CMD58, 0);
 
   if (response != SD_R1_READY_STATE) {
     return false;
   }
 
   // Read OCR register
   uint32_t ocr = _spi->transfer(0xFF) << 24;
   ocr |= _spi->transfer(0xFF) << 16;
   ocr |= _spi->transfer(0xFF) << 8;
   ocr |= _spi->transfer(0xFF);
 
   // Check CCS bit (bit 30)
   _isSDHC = (ocr & 0x40000000) != 0;
 
   // Deselect card and send extra clock pulses
   digitalWrite(_csPin, HIGH);
   _spi->transfer(0xFF);
 
   return true;
 }
 
 // Modified sendCommand function to use the stored SPI interface
 uint8_t ZBSdCard::sendCommand(uint8_t cmd, uint32_t arg) {
  uint8_t crc = 0xFF;  // Default CRC

  // Special cases for CRC
  if (cmd == SD_CMD0) crc = 0x95;
  if (cmd == SD_CMD8) crc = 0x87;

  _spi->transfer(0xFF);  // Dummy byte
  digitalWrite(_csPin, LOW);
  delayMicroseconds(10);

  // Send command
  _spi->transfer(0x40 | cmd);  // Commands always start with 01

  // SDHC cards use block addressing for data commands
  if (_isSDHC && (cmd == SD_CMD17 || cmd == SD_CMD18 || cmd == SD_CMD24 || cmd == SD_CMD25)) {
    _spi->transfer(arg >> 24);
    _spi->transfer(arg >> 16);
    _spi->transfer(arg >> 8);
    _spi->transfer(arg);
  } else {
    _spi->transfer((arg >> 24) & 0xFF);
    _spi->transfer((arg >> 16) & 0xFF);
    _spi->transfer((arg >> 8) & 0xFF);
    _spi->transfer(arg & 0xFF);
  }

  _spi->transfer(crc);

  // Wait for response
  uint8_t r1;
  for (int i = 0; i < 10; i++) {
    r1 = _spi->transfer(0xFF);
    if (!(r1 & 0x80)) break;  // Exit once we get a response
  }

  return r1;
}
 
 // Function to send an application-specific command to the SD card
 uint8_t ZBSdCard::sendAppCommand(uint8_t cmd, uint32_t arg) {
   uint8_t r1 = sendCommand(SD_CMD55, 0);
   if (r1 > 1) {
     return r1;  // Error
   }
   return sendCommand(cmd, arg);
 }
 
 // Optimized readBlock function
 bool ZBSdCard::readBlock(uint32_t blockAddr, uint8_t *data) {
   uint8_t response;
   uint16_t timeout = 0;
 
   // For SDHC/SDXC cards, address is already a block address
   uint32_t readAddr = _isSDHC ? blockAddr : blockAddr * 512;
 
   // Multiple attempts for reliability
   for (int attempts = 0; attempts < 3; attempts++) {
     response = sendCommand(SD_CMD17, readAddr);
 
     if (response != SD_R1_READY_STATE) {
       digitalWrite(_csPin, HIGH);
       delay(10);  // Add delay before retry
       continue;   // Try again
     }
 
     // Wait for data token
     timeout = 0;
     do {
       response = _spi->transfer(0xFF);
       timeout++;
       if (timeout > 10000) {  // More reasonable timeout
         digitalWrite(_csPin, HIGH);
         delay(20);  // Add delay before retry
         break;      // Exit inner loop but retry command
       }
     } while (response != SD_DATA_START_BLOCK);
 
     if (response != SD_DATA_START_BLOCK) {
       continue;
     }
 
     // Read data
 #ifdef ZBSD_MINIMAL_MODE
     // Use smaller buffer for CH32V003, reading 256 bytes at a time
     for (int i = 0; i < BLOCK_SIZE; i++) {
       // Only store data if it's within our buffer size
       if (i < ZBSD_BUFFER_SIZE) {
         data[i] = _spi->transfer(0xFF);
       } else {
         // Still need to read all 512 bytes to complete the block read
         _spi->transfer(0xFF);
       }
     }
 #else
     // Full 512-byte reads for CH32V203
     for (int i = 0; i < BLOCK_SIZE; i++) {
       data[i] = _spi->transfer(0xFF);
     }
 #endif
 
     // Read and ignore CRC
     _spi->transfer(0xFF);
     _spi->transfer(0xFF);
 
     digitalWrite(_csPin, HIGH);
     _spi->transfer(0xFF);  // One extra byte
     
     return true;  // Success
   }
 
   digitalWrite(_csPin, HIGH);
   return false;
 }
 
 // Optimized writeBlock function 
 bool ZBSdCard::writeBlock(uint32_t blockAddr, const uint8_t *data) {
   uint8_t response;
   uint16_t retry;
 
   // For SDHC/SDXC cards, address is already a block address
   uint32_t writeAddr = _isSDHC ? blockAddr : blockAddr * 512;
 
   // Multiple attempts for reliability
   for (int attempts = 0; attempts < 3; attempts++) {
     response = sendCommand(SD_CMD24, writeAddr);
 
     if (response != SD_R1_READY_STATE) {
       digitalWrite(_csPin, HIGH);
       delay(10);  // Add delay before retry
       continue;   // Try again
     }
 
     // Wait before sending data
     _spi->transfer(0xFF);
 
     // Send data token
     _spi->transfer(SD_DATA_START_BLOCK);
 
 #ifdef ZBSD_MINIMAL_MODE
     // In minimal mode, handle partial buffer
     for (int i = 0; i < ZBSD_BUFFER_SIZE; i++) {
       _spi->transfer(data[i]);
     }
     
     // For the remaining bytes, send zeros to complete the 512-byte block
     for (int i = ZBSD_BUFFER_SIZE; i < BLOCK_SIZE; i++) {
       _spi->transfer(0);
     }
 #else
     // Full 512-byte write
     for (int i = 0; i < BLOCK_SIZE; i++) {
       _spi->transfer(data[i]);
     }
 #endif
 
     // Send dummy CRC
     _spi->transfer(0xFF);
     _spi->transfer(0xFF);
 
     // Get response
     response = _spi->transfer(0xFF);
 
     if ((response & SD_DATA_RES_MASK) != SD_DATA_RES_ACCEPTED) {
       digitalWrite(_csPin, HIGH);
       delay(20);  // Longer delay before retry
       continue;   // Try again
     }
 
     // Wait for write to complete
     retry = 0;
     while (_spi->transfer(0xFF) == 0x00) {
       retry++;
       if (retry >= 10000) {  // More reasonable timeout
         digitalWrite(_csPin, HIGH);
         delay(50);  // Add significant delay before next attempt
         break;      // Exit inner loop but might retry
       }
       delayMicroseconds(100);  // Less aggressive polling
     }
 
     if (retry < 10000) {  // Write completed successfully
       digitalWrite(_csPin, HIGH);
       _spi->transfer(0xFF);  // One extra byte
       return true;
     }
   }
 
   digitalWrite(_csPin, HIGH);
   return false;
 }
 
 // Find the partition information from MBR
 bool ZBSdCard::findPartition() {
   // Read MBR (Master Boot Record) - always at sector 0
   if (!readBlock(0, buffer)) {
     return false;
   }
 
   // Check for MBR signature at offset 510-511
   if (buffer[510] != 0x55 || buffer[511] != 0xAA) {
     return false;
   }
 
   // Parse the first partition entry at offset 446
   PartitionEntry *partition = (PartitionEntry *)(buffer + 446);
 
   // Check if partition type is FAT32 (0x0B or 0x0C)
   if (partition->type != 0x0B && partition->type != 0x0C) {
     return false;
   }
 
   // Get the starting sector of the partition
   _partitionStartSector = partition->first_sector;
 
   return true;
 }
 
 // Read the FAT32 boot sector to get filesystem parameters
 bool ZBSdCard::readFAT32BootSector() {
   // Read the first sector of the partition
   if (!readBlock(_partitionStartSector, buffer)) {
     return false;
   }
 
   // Cast buffer to FAT32BootSector structure
   FAT32BootSector *boot = (FAT32BootSector *)buffer;
 
   // Check for valid boot sector signature
   if (buffer[510] != 0x55 || buffer[511] != 0xAA) {
     return false;
   }
 
   // Get filesystem parameters
   _sectorsPerCluster = boot->sectors_per_cluster;
 
   // Calculate important sector numbers
   _fatStartSector = _partitionStartSector + boot->reserved_sector_count;
   _rootDirCluster = boot->root_cluster;
 
   // Number of sectors per FAT
   uint32_t fatSize = boot->fat_size_32;
 
   // Calculate data sector start
   _dataStartSector = _fatStartSector + (boot->fat_count * fatSize);
 
   // Calculate cluster size in bytes
   _clusterSize = _sectorsPerCluster * BLOCK_SIZE;
 
   return true;
 }
 
 // Function to get the sector number for a given cluster
 uint32_t ZBSdCard::clusterToSector(uint32_t cluster) {
   // First data cluster is numbered as 2
   return _dataStartSector + ((cluster - 2) * _sectorsPerCluster);
 }
 
 // Read a FAT entry
 uint32_t ZBSdCard::readFATEntry(uint32_t cluster) {
   uint32_t fatOffset = cluster * 4;
   uint32_t fatSector = _fatStartSector + (fatOffset / BLOCK_SIZE);
   uint32_t entryOffset = fatOffset % BLOCK_SIZE;
 
   if (!readBlock(fatSector, buffer)) {
     return 0x0FFFFFF7;  // Bad cluster
   }
 
   uint32_t entry = *((uint32_t *)(buffer + entryOffset));
   return entry & 0x0FFFFFFF;  // Mask out the upper 4 bits
 }
 
 // Write a FAT entry
 bool ZBSdCard::writeFATEntry(uint32_t cluster, uint32_t value) {
   uint32_t fatOffset = cluster * 4;
   uint32_t fatSector = _fatStartSector + (fatOffset / BLOCK_SIZE);
   uint32_t entryOffset = fatOffset % BLOCK_SIZE;
 
   // Read the current FAT sector
   if (!readBlock(fatSector, buffer)) {
     return false;
   }
 
   // Modify the value (keep upper 4 bits intact)
   uint32_t currentValue = *((uint32_t *)(buffer + entryOffset));
   uint32_t newValue = (currentValue & 0xF0000000) | (value & 0x0FFFFFFF);
   *((uint32_t *)(buffer + entryOffset)) = newValue;
 
   // Write the modified sector back
   if (!writeBlock(fatSector, buffer)) {
     return false;
   }
 
   return true;
 }
 
 // Find a free cluster
 uint32_t ZBSdCard::findFreeCluster() {
   uint32_t cluster = _firstFreeFAT;
   uint32_t maxCluster = 0x0FFFFFF0;  // Max valid cluster number
 
   while (cluster < maxCluster) {
     uint32_t entry = readFATEntry(cluster);
     if (entry == 0) {
       // Found a free cluster
       _firstFreeFAT = cluster + 1;
       return cluster;
     }
     cluster++;
   }
 
   return 0;  // No free clusters found
 }
 
 // Convert filename to FAT 8.3 format
 void ZBSdCard::filenameToFAT83(const char *filename, char *fatName) {
   memset(fatName, ' ', 11);  // Fill with spaces
 
   int i = 0;
   int j = 0;
   int dotPos = -1;
 
   // First, find the dot position if any
   for (int k = 0; filename[k]; k++) {
     if (filename[k] == '.') {
       dotPos = k;
     }
   }
 
   // Process basename (up to 8 chars or until dot)
   while (filename[i] && j < 8) {
     if (filename[i] == '.') {
       // Found a dot, skip to extension handling
       break;
     }
 
     if (filename[i] == ' ') {
       // Skip spaces in 8.3 name
       i++;
       continue;
     }
 
     char c = toupper(filename[i]);
     if (c != ' ' && c != '.') {
       fatName[j] = c;
       j++;
     }
     i++;
   }
 
   // If we hit a dot, skip to extension handling
   if (filename[i] == '.') {
     i++;  // Skip the dot
   } else {
     // Skip to the extension after the dot if we found one earlier
     if (dotPos >= 0) {
       i = dotPos + 1;
     }
   }
 
   // Process extension (max 3 chars)
   j = 8;  // Start at extension position
   while (filename[i] && j < 11) {
     char c = toupper(filename[i]);
     if (c != ' ' && c != '.') {
       fatName[j] = c;
       j++;
     }
     i++;
   }
 }
 
 // Find an entry in a directory
 uint32_t ZBSdCard::findDirEntry(uint32_t dirCluster, const char *name, DirectoryEntry *entry) {
   char fatName[11];
   filenameToFAT83(name, fatName);
 
   // Follow the cluster chain
   while (dirCluster >= 2 && dirCluster < 0x0FFFFFF0) {
     uint32_t sector = clusterToSector(dirCluster);
 
     // Read all sectors in this cluster
     for (uint32_t i = 0; i < _sectorsPerCluster; i++) {
       if (!readBlock(sector + i, buffer)) {
         return 0;
       }
 
       // Check all directory entries in this sector
       DirectoryEntry *dir = (DirectoryEntry *)buffer;
       for (int j = 0; j < BLOCK_SIZE / sizeof(DirectoryEntry); j++) {
         if (dir[j].name[0] == 0) {
           // End of directory
           return 0;
         }
 
         if (dir[j].name[0] != 0xE5) {  // Not deleted
           // Compare name
           if (memcmp(dir[j].name, fatName, 11) == 0) {
             // Found the entry
             if (entry) {
               *entry = dir[j];
             }
             return ((sector + i) << 16) | j;  // Return position (sector << 16 | entry_index)
           }
         }
       }
     }
 
     // Move to next cluster in chain
     dirCluster = readFATEntry(dirCluster);
   }
 
   return 0;  // Not found
 }
 
 // Find an entry in the root directory
 uint32_t ZBSdCard::findInRootDir(const char *name, DirectoryEntry *entry) {
   return findDirEntry(_rootDirCluster, name, entry);
 }
 
 // Write a directory entry
 bool ZBSdCard::writeDirEntry(uint32_t entryPos, const DirectoryEntry *entry) {
   uint32_t sector = entryPos >> 16;
   uint32_t index = entryPos & 0xFFFF;
 
   // Read the sector containing the directory entry
   if (!readBlock(sector, buffer)) {
     return false;
   }
 
   // Update the entry
   DirectoryEntry *dir = (DirectoryEntry *)buffer;
   dir[index] = *entry;
 
   // Write back the sector
   return writeBlock(sector, buffer);
 }
 
 // Find a free directory entry
 uint32_t ZBSdCard::findFreeDirEntry(uint32_t dirCluster) {
   // Follow the cluster chain
   while (dirCluster >= 2 && dirCluster < 0x0FFFFFF0) {
     uint32_t sector = clusterToSector(dirCluster);
 
     // Read all sectors in this cluster
     for (uint32_t i = 0; i < _sectorsPerCluster; i++) {
       if (!readBlock(sector + i, buffer)) {
         return 0;
       }
 
       // Check all directory entries in this sector
       DirectoryEntry *dir = (DirectoryEntry *)buffer;
       for (int j = 0; j < BLOCK_SIZE / sizeof(DirectoryEntry); j++) {
         if (dir[j].name[0] == 0 || dir[j].name[0] == 0xE5) {
           // Found a free entry
           return ((sector + i) << 16) | j;
         }
       }
     }
 
     // If we get here, need to allocate a new cluster
     uint32_t nextCluster = readFATEntry(dirCluster);
     if (nextCluster >= 0x0FFFFFF0) {
       // End of chain, allocate a new cluster
       nextCluster = findFreeCluster();
       if (nextCluster == 0) {
         return 0;  // No free clusters
       }
 
       // Link to the chain
       writeFATEntry(dirCluster, nextCluster);
       writeFATEntry(nextCluster, 0x0FFFFFFF);  // Mark as end of chain
 
       // Clear the new cluster
       uint32_t newSector = clusterToSector(nextCluster);
       memset(buffer, 0, ZBSD_BUFFER_SIZE);  // Use appropriate buffer size
       
       // Write all sectors in the new cluster
       for (uint32_t i = 0; i < _sectorsPerCluster; i++) {
         if (!writeBlock(newSector + i, buffer)) {
           return 0;
         }
       }
 
       // Return the first entry in the new cluster
       return (newSector << 16) | 0;
     }
 
     dirCluster = nextCluster;
   }
 
   return 0;  // No free entries
 }
 
 // Create a new entry in the directory
 uint32_t ZBSdCard::createDirEntry(uint32_t dirCluster, const char *name, uint8_t attributes, uint32_t fileCluster, uint32_t fileSize) {
   char fatName[11];
   filenameToFAT83(name, fatName);
 
   // First check if the entry already exists
   if (findDirEntry(dirCluster, name, nullptr)) {
     return 0;
   }
 
   // Find a free directory entry
   uint32_t entryPos = findFreeDirEntry(dirCluster);
   if (entryPos == 0) {
     return 0;
   }
 
   uint32_t sector = entryPos >> 16;
   uint32_t index = entryPos & 0xFFFF;
 
   // Read the sector containing the directory entry
   if (!readBlock(sector, buffer)) {
     return 0;
   }
 
   // Create the directory entry
   DirectoryEntry *dir = (DirectoryEntry *)buffer;
   memcpy(dir[index].name, fatName, 11);
   dir[index].attributes = attributes;
 
   // Set current time/date (dummy values for simplicity)
   uint16_t date = (2023 - 1980) << 9 | 5 << 5 | 2;  // 2023-05-02
   uint16_t time = 12 << 11 | 0 << 5 | 0 / 2;        // 12:00:00
 
   dir[index].create_date = date;
   dir[index].create_time = time;
   dir[index].create_time_tenth = 0;
   dir[index].write_date = date;
   dir[index].write_time = time;
   dir[index].last_access_date = date;
 
   dir[index].first_cluster_high = (fileCluster >> 16) & 0xFFFF;
   dir[index].first_cluster_low = fileCluster & 0xFFFF;
   dir[index].file_size = fileSize;
 
   // Write the sector back
   if (!writeBlock(sector, buffer)) {
     return 0;
   }
 
   return entryPos;
 }
 
 // Write data to a file cluster chain
 bool ZBSdCard::writeFile(uint32_t cluster, const char *data, uint32_t size, bool append) {
   uint32_t remainingBytes = size;
   uint32_t currentCluster = cluster;
   const char *currentData = data;
   uint32_t currentOffset = 0;  // Track position within current cluster
 
   // If appending, determine position to write
   if (append) {
     // Follow cluster chain to the end
     uint32_t lastCluster = currentCluster;
     uint32_t nextCluster;
     
     while ((nextCluster = readFATEntry(lastCluster)) < 0x0FFFFFF0) {
       lastCluster = nextCluster;
     }
     
     currentCluster = lastCluster;
     
     // Calculate offset in last cluster based on file size
     uint32_t bytesInPrevClusters = 0;
     uint32_t tempCluster = cluster;
     
     while (tempCluster != currentCluster) {
       bytesInPrevClusters += _clusterSize;
       tempCluster = readFATEntry(tempCluster);
     }
     
     // Assume file size is correct (for minimal implementation)
     currentOffset = size - bytesInPrevClusters;
     if (currentOffset > _clusterSize) {
       currentOffset = 0;  // Reset if something went wrong
     }
   }
 
   // Write the data using smaller buffers for better memory usage
   while (remainingBytes > 0) {
     uint32_t sector = clusterToSector(currentCluster);
     uint32_t sectorOffset = currentOffset / BLOCK_SIZE;
     uint32_t byteOffset = currentOffset % BLOCK_SIZE;
     
     // Process sectors one at a time to minimize RAM usage
     for (uint32_t i = sectorOffset; i < _sectorsPerCluster && remainingBytes > 0; i++) {
       uint8_t writeBuffer[ZBSD_BUFFER_SIZE];
       
       // If partial sector, read it first
       if (byteOffset > 0 || (remainingBytes < ZBSD_BUFFER_SIZE && byteOffset == 0)) {
         if (!readBlock(sector + i, writeBuffer)) {
           return false;
         }
       } else {
         // Clear buffer for full sector write
         memset(writeBuffer, 0, ZBSD_BUFFER_SIZE);
       }
       
       // Calculate bytes to write in this sector
       uint32_t bytesToWrite = ZBSD_BUFFER_SIZE - byteOffset;
       if (bytesToWrite > remainingBytes) {
         bytesToWrite = remainingBytes;
       }
       
       // Copy data to buffer
       memcpy(writeBuffer + byteOffset, currentData, bytesToWrite);
       
       // Write sector - handle partial writes for CH32V003
       if (!writeBlock(sector + i, writeBuffer)) {
         return false;
       }
       
       // Update counters
       currentData += bytesToWrite;
       remainingBytes -= bytesToWrite;
       currentOffset += bytesToWrite;
       byteOffset = 0;  // Reset byte offset for next sector
     }
     
     // If we still have data to write, move to next cluster
     if (remainingBytes > 0) {
       uint32_t nextCluster = readFATEntry(currentCluster);
       
       // Check if we need a new cluster
       if (nextCluster >= 0x0FFFFFF0) {
         nextCluster = findFreeCluster();
         if (nextCluster == 0) {
           return false;  // No free clusters
         }
         
         // Link to the chain
         writeFATEntry(currentCluster, nextCluster);
         writeFATEntry(nextCluster, 0x0FFFFFFF);  // End of chain
       }
       
       currentCluster = nextCluster;
       currentOffset = 0;  // Reset offset for new cluster
     }
   }
   
   return true;
 }
 
 // Find a file in the root directory
 uint32_t ZBSdCard::findFile(const char *filename, DirectoryEntry *entry) {
   DirectoryEntry dirEntry;
   if (findInRootDir(filename, &dirEntry)) {
     // Check if it's a file (not a directory)
     if (!(dirEntry.attributes & SD_ATTR_DIRECTORY)) {
       if (entry) {
         *entry = dirEntry;
       }
       return ((uint32_t)dirEntry.first_cluster_high << 16) | dirEntry.first_cluster_low;
     }
   }
   return 0;  // File not found
 }
 
 // Simplified function to find a directory by path for minimal mode
 uint32_t ZBSdCard::findDirectoryByPath(const char *path) {
 #ifdef ZBSD_MINIMAL_MODE
   // In minimal mode, we only support root directory
   return _rootDirCluster;
 #else
   char pathCopy[100];
   strncpy(pathCopy, path, 99);
   pathCopy[99] = 0;
 
   // Remove leading slash if present
   char *currentPath = pathCopy;
   if (currentPath[0] == '/') {
     currentPath++;
   }
 
   // Empty path means root directory
   if (strlen(currentPath) == 0) {
     return _rootDirCluster;
   }
 
   // Start from root directory
   uint32_t currentCluster = _rootDirCluster;
 
   // Process each directory in the path
   char *segment = strtok(currentPath, "/");
   while (segment && strlen(segment) > 0) {
     // Skip empty segments
     if (strlen(segment) == 0) {
       segment = strtok(NULL, "/");
       continue;
     }
 
     // Find this segment in the current directory
     DirectoryEntry dirEntry;
     if (!findDirEntry(currentCluster, segment, &dirEntry)) {
       // Directory not found
       _lastError = SD_ERROR_NO_PATH;
       return 0;
     }
 
     // Check if it's a directory
     if (!(dirEntry.attributes & SD_ATTR_DIRECTORY)) {
       // Not a directory
       _lastError = SD_ERROR_NO_PATH;
       return 0;
     }
 
     // Move to this directory
     currentCluster = ((uint32_t)dirEntry.first_cluster_high << 16) | dirEntry.first_cluster_low;
 
     // Get next segment
     segment = strtok(NULL, "/");
   }
 
   return currentCluster;
 #endif
 }
 
 //------------------------------------------------------------------
 // Public methods - Core file operations for all platforms
 //------------------------------------------------------------------
 
 // Create a file in the root directory
 bool ZBSdCard::createFile(const char *fileName, const char *content) {
   _lastError = SD_SUCCESS;
   uint32_t size = strlen(content);
 
   // Check if file already exists
   DirectoryEntry fileEntry;
   uint32_t existingPos = findInRootDir(fileName, &fileEntry);
   
   if (existingPos != 0) {
     // File exists - update it
     uint32_t fileCluster = ((uint32_t)fileEntry.first_cluster_high << 16) | fileEntry.first_cluster_low;
     
     // Update file size
     fileEntry.file_size = size;
     
     // Update directory entry
     if (!writeDirEntry(existingPos, &fileEntry)) {
       _lastError = SD_ERROR_DISK_WRITE;
       return false;
     }
     
     // Write new content
     if (size > 0) {
       if (!writeFile(fileCluster, content, size, false)) {
         _lastError = SD_ERROR_DISK_WRITE;
         return false;
       }
     }
     
     return true;
   }
   
   // Allocate a cluster for the new file
   uint32_t fileCluster = findFreeCluster();
   if (fileCluster == 0) {
     _lastError = SD_ERROR_DISK_WRITE;
     return false;
   }
 
   // Mark the cluster as end of chain
   writeFATEntry(fileCluster, 0x0FFFFFFF);
 
   // Create directory entry in root directory
   if (!createDirEntry(_rootDirCluster, fileName, SD_ATTR_ARCHIVE, fileCluster, size)) {
     _lastError = SD_ERROR_DISK_WRITE;
     return false;
   }
 
   // Write content to the file
   if (size > 0) {
     if (!writeFile(fileCluster, content, size, false)) {
       _lastError = SD_ERROR_DISK_WRITE;
       return false;
     }
   }
   
   return true;
 }
 
 // Append data to an existing file
 bool ZBSdCard::appendFile(const char *fileName, const char *content) {
   _lastError = SD_SUCCESS;
   
   // Find file in root directory
   DirectoryEntry fileEntry;
   uint32_t entryPos = findInRootDir(fileName, &fileEntry);
   
   if (entryPos == 0) {
     // File doesn't exist, create it
     return createFile(fileName, content);
   }
   
   // Get file info
   uint32_t fileCluster = ((uint32_t)fileEntry.first_cluster_high << 16) | fileEntry.first_cluster_low;
   uint32_t currentSize = fileEntry.file_size;
   uint32_t appendSize = strlen(content);
   uint32_t newSize = currentSize + appendSize;
   
   // Update file size in directory entry
   fileEntry.file_size = newSize;
   if (!writeDirEntry(entryPos, &fileEntry)) {
     _lastError = SD_ERROR_DISK_WRITE;
     return false;
   }
   
   // Write the new data starting at the end
   return writeFile(fileCluster, content, appendSize, true);
 }
 
 // Delete a file
 bool ZBSdCard::deleteFile(const char *fileName) {
   _lastError = SD_SUCCESS;
   
   // Find the file in the root directory
   DirectoryEntry fileEntry;
   uint32_t entryPos = findInRootDir(fileName, &fileEntry);
   
   if (entryPos == 0) {
     _lastError = SD_ERROR_NO_FILE;
     return false;
   }
   
   // Check if it's actually a file
   if (fileEntry.attributes & SD_ATTR_DIRECTORY) {
     _lastError = SD_ERROR_NO_FILE;
     return false;
   }
   
   // Get the starting cluster of the file
   uint32_t fileCluster = ((uint32_t)fileEntry.first_cluster_high << 16) | fileEntry.first_cluster_low;
   
   // Free all clusters used by the file
   uint32_t currentCluster = fileCluster;
   uint32_t nextCluster;
   
   while (currentCluster >= 2 && currentCluster < 0x0FFFFFF0) {
     nextCluster = readFATEntry(currentCluster);
     
     // Mark the current cluster as free
     if (!writeFATEntry(currentCluster, 0)) {
       _lastError = SD_ERROR_DISK_WRITE;
       return false;
     }
     
     currentCluster = nextCluster;
   }
   
   // Mark the directory entry as deleted (set first byte to 0xE5)
   uint32_t sector = entryPos >> 16;
   uint32_t index = entryPos & 0xFFFF;
   
   // Read the sector containing the directory entry
   if (!readBlock(sector, buffer)) {
     _lastError = SD_ERROR_DISK_READ;
     return false;
   }
   
   // Update the entry
   DirectoryEntry *dir = (DirectoryEntry *)buffer;
   dir[index].name[0] = 0xE5;  // Mark as deleted
   
   // Write back the sector
   if (!writeBlock(sector, buffer)) {
     _lastError = SD_ERROR_DISK_WRITE;
     return false;
   }
   
   return true;
 }
 
 // Optimized readFile function that works with smaller buffers
 uint32_t ZBSdCard::readFile(const char *fileName, char *buffer, uint32_t bufferSize) {
   _lastError = SD_SUCCESS;
   
 #ifdef ZBSD_MINIMAL_MODE
   // Simpler path handling in minimal mode - use root directory only
   DirectoryEntry fileEntry;
   if (!findInRootDir(fileName, &fileEntry)) {
     _lastError = SD_ERROR_NO_FILE;
     return 0;
   }
 #else
   // Parse the path to extract directory path and filename
   char pathCopy[100];
   strncpy(pathCopy, fileName, 99);
   pathCopy[99] = 0;
   
   // Find the last slash to separate directory and filename
   char *lastSlash = strrchr(pathCopy, '/');
   char *dirPath;
   char *fileNameOnly;
   uint32_t dirCluster;
   
   if (!lastSlash) {
     // No directory specified, file is in root
     dirPath = "/";
     fileNameOnly = pathCopy;
     dirCluster = _rootDirCluster;
   } else {
     // Split the path
     *lastSlash = 0;  // Null-terminate the directory path
     dirPath = pathCopy;
     fileNameOnly = lastSlash + 1;
     
     // Handle empty directory path (root)
     if (strlen(dirPath) == 0) {
       dirPath = "/";
       dirCluster = _rootDirCluster;
     } else {
       // Find the directory
       dirCluster = findDirectoryByPath(dirPath);
       if (dirCluster == 0) {
         _lastError = SD_ERROR_NO_PATH;
         return 0;
       }
     }
   }
   
   // Find the file in the directory
   DirectoryEntry fileEntry;
   if (!findDirEntry(dirCluster, fileNameOnly, &fileEntry)) {
     _lastError = SD_ERROR_NO_FILE;
     return 0;
   }
 #endif
 
   // Check if it's actually a file (not a directory)
   if (fileEntry.attributes & SD_ATTR_DIRECTORY) {
     _lastError = SD_ERROR_NO_FILE;
     return 0;
   }
   
   // Get file info
   uint32_t fileCluster = ((uint32_t)fileEntry.first_cluster_high << 16) | fileEntry.first_cluster_low;
   uint32_t fileSize = fileEntry.file_size;
   
   // Make sure buffer is large enough
   if (bufferSize <= fileSize) {
     // Can't read entire file
     fileSize = bufferSize - 1; // Leave room for null terminator
   }
   
   // Clear the buffer first
   memset(buffer, 0, bufferSize);
   
   // Read the file data - use smaller temporary buffer for RAM-constrained devices
   uint32_t bytesRead = 0;
   uint32_t currentCluster = fileCluster;
   
   while (bytesRead < fileSize && currentCluster >= 2 && currentCluster < 0x0FFFFFF0) {
     uint32_t sector = clusterToSector(currentCluster);
     
     // Process all sectors in this cluster
     for (uint32_t i = 0; i < _sectorsPerCluster && bytesRead < fileSize; i++) {
       // Read the sector into our buffer
       if (!readBlock(sector + i, this->buffer)) {
         _lastError = SD_ERROR_DISK_READ;
         buffer[bytesRead] = 0;  // Null terminate what we've read so far
         return bytesRead;
       }
       
       // Calculate how many bytes to copy from this sector
       uint32_t remainingBytes = fileSize - bytesRead;
       uint32_t bytesToCopy = (remainingBytes < ZBSD_BUFFER_SIZE) ? remainingBytes : ZBSD_BUFFER_SIZE;
       
       // Copy data to the output buffer
       memcpy(buffer + bytesRead, this->buffer, bytesToCopy);
       bytesRead += bytesToCopy;
     }
     
     // Follow the cluster chain
     if (bytesRead < fileSize) {
       currentCluster = readFATEntry(currentCluster);
     }
   }
   
   // Ensure null termination
   buffer[bytesRead] = 0;
   
   return bytesRead;
 }
 
 // List directory function
 bool ZBSdCard::listDirectory(const char *path) {
   _lastError = SD_SUCCESS;
   
   // Get the directory cluster for the specified path
   uint32_t dirCluster = findDirectoryByPath(path);
   if (dirCluster == 0) {
     // Path not found
     _lastError = SD_ERROR_NO_PATH;
     return false;
   }
   
   // Follow the cluster chain - but don't actually display anything in this
   // minimal implementation to save memory
   while (dirCluster >= 2 && dirCluster < 0x0FFFFFF0) {
     uint32_t sector = clusterToSector(dirCluster);
     
     // Read all sectors in this cluster
     for (uint32_t i = 0; i < _sectorsPerCluster; i++) {
       if (!readBlock(sector + i, buffer)) {
         _lastError = SD_ERROR_DISK_READ;
         return false;
       }
     }
     
     // Move to next cluster in chain
     dirCluster = readFATEntry(dirCluster);
   }
   
   return true;
 }
 
 // New function to get directory entries
 bool ZBSdCard::listDirectoryEntries(const char* path, char (*fileNames)[20], bool* isDirectory, int maxEntries, int* entryCount) {
   _lastError = SD_SUCCESS;
   *entryCount = 0;
   
   // Get the directory cluster for the specified path
   uint32_t dirCluster = findDirectoryByPath(path);
   if (dirCluster == 0) {
     // Path not found
     _lastError = SD_ERROR_NO_PATH;
     return false;
   }
   
   // Follow the cluster chain
   while (dirCluster >= 2 && dirCluster < 0x0FFFFFF0 && *entryCount < maxEntries) {
     uint32_t sector = clusterToSector(dirCluster);
     
     // Read all sectors in this cluster
     for (uint32_t i = 0; i < _sectorsPerCluster && *entryCount < maxEntries; i++) {
       if (!readBlock(sector + i, buffer)) {
         _lastError = SD_ERROR_DISK_READ;
         return false;
       }
       
       // Check all directory entries in this sector
       DirectoryEntry *dir = (DirectoryEntry *)buffer;
       for (int j = 0; j < BLOCK_SIZE / sizeof(DirectoryEntry) && *entryCount < maxEntries; j++) {
         // End of directory entries
         if (dir[j].name[0] == 0) {
           break;
         }
         
         // Skip deleted entries (first byte is 0xE5)
         if ((uint8_t)dir[j].name[0] == 0xE5) {
           continue;
         }
         
         // Skip LFN entries and system/hidden files
         if ((dir[j].attributes & SD_ATTR_LONG_NAME) == SD_ATTR_LONG_NAME ||
             (dir[j].attributes & (SD_ATTR_HIDDEN | SD_ATTR_SYSTEM | SD_ATTR_VOLUME_ID)) != 0) {
           continue;
         }
         
         // Skip "." and ".." entries
         if (dir[j].name[0] == '.' && (dir[j].name[1] == ' ' || 
             (dir[j].name[1] == '.' && dir[j].name[2] == ' '))) {
           continue;
         }
         
         // Convert 8.3 filename back to readable format
         char name[13] = {0};  // 8 chars + . + 3 chars + null
         int namePos = 0;
         
         // Process basename
         for (int k = 0; k < 8 && dir[j].name[k] != ' '; k++) {
           name[namePos++] = dir[j].name[k];
         }
         
         // Process extension if present
         if (dir[j].ext[0] != ' ') {
           name[namePos++] = '.';
           for (int k = 0; k < 3 && dir[j].ext[k] != ' '; k++) {
             name[namePos++] = dir[j].ext[k];
           }
         }
         
         name[namePos] = 0;  // Null terminate
         
         // Store the entry info
         strncpy(fileNames[*entryCount], name, 19);
         fileNames[*entryCount][19] = 0;  // Ensure null termination
         isDirectory[*entryCount] = (dir[j].attributes & SD_ATTR_DIRECTORY) != 0;
         
         (*entryCount)++;
       }
     }
     
     // Move to next cluster in chain
     dirCluster = readFATEntry(dirCluster);
   }
   
   return true;
 }
 
 // Get error message
 const char* ZBSdCard::getErrorMessage() {
   switch (_lastError) {
     case SD_SUCCESS:
       return "No error";
     case SD_ERROR_INIT:
       return "SD card initialization failed";
     case SD_ERROR_DISK_READ:
       return "Disk read error";
     case SD_ERROR_DISK_WRITE:
       return "Disk write error";
     case SD_ERROR_FAT_READ:
       return "FAT read error";
     case SD_ERROR_NO_FILE:
       return "File not found";
     case SD_ERROR_NO_PATH:
       return "Path not found";
     default:
       return "Unknown error";
   }
 }
 
 //------------------------------------------------------------------
 // Extended methods - only compiled for CH32V203
 //------------------------------------------------------------------
 
 #ifndef ZBSD_MINIMAL_MODE
 
 // Create a directory in the root directory
 bool ZBSdCard::createDirectory(const char *dirname) {
   _lastError = SD_SUCCESS;
   
   // Check if directory already exists
   DirectoryEntry existingDir;
   uint32_t existingPos = findInRootDir(dirname, &existingDir);
   if (existingPos != 0) {
     // Directory exists already, but we should check if it's actually a directory
     if (existingDir.attributes & SD_ATTR_DIRECTORY) {
       return true; // Not an error, directory exists already
     } else {
       // Exists but is not a directory
       _lastError = SD_ERROR_NO_PATH;
       return false;
     }
   }
 
   // Allocate a cluster for the new directory
   uint32_t dirCluster = findFreeCluster();
   if (dirCluster == 0) {
     _lastError = SD_ERROR_DISK_WRITE;
     return false;
   }
 
   // Mark the cluster as end of chain
   writeFATEntry(dirCluster, 0x0FFFFFFF);
 
   // Initialize the directory cluster with "." and ".." entries
   uint32_t dirSector = clusterToSector(dirCluster);
 
   // Clear the entire cluster
   memset(buffer, 0, ZBSD_BUFFER_SIZE);
   for (uint32_t i = 0; i < _sectorsPerCluster; i++) {
     if (!writeBlock(dirSector + i, buffer)) {
       _lastError = SD_ERROR_DISK_WRITE;
       return false;
     }
   }
 
   // Create "." entry (points to itself)
   memset(buffer, 0, ZBSD_BUFFER_SIZE);
   DirectoryEntry *dir = (DirectoryEntry *)buffer;
 
   // "." entry
   memset(dir[0].name, ' ', 11);
   dir[0].name[0] = '.';
   dir[0].attributes = SD_ATTR_DIRECTORY;
 
   // Set time/date
   uint16_t date = (2023 - 1980) << 9 | 5 << 5 | 2;  // 2023-05-02
   uint16_t time = 12 << 11 | 0 << 5 | 0 / 2;        // 12:00:00
 
   dir[0].create_date = date;
   dir[0].create_time = time;
   dir[0].create_time_tenth = 0;
   dir[0].write_date = date;
   dir[0].write_time = time;
   dir[0].last_access_date = date;
 
   dir[0].first_cluster_high = (dirCluster >> 16) & 0xFFFF;
   dir[0].first_cluster_low = dirCluster & 0xFFFF;
   dir[0].file_size = 0;
 
   // ".." entry (points to parent directory, in this case root)
   memset(dir[1].name, ' ', 11);
   dir[1].name[0] = '.';
   dir[1].name[1] = '.';
   dir[1].attributes = SD_ATTR_DIRECTORY;
 
   dir[1].create_date = date;
   dir[1].create_time = time;
   dir[1].create_time_tenth = 0;
   dir[1].write_date = date;
   dir[1].write_time = time;
   dir[1].last_access_date = date;
 
   dir[1].first_cluster_high = (_rootDirCluster >> 16) & 0xFFFF;
   dir[1].first_cluster_low = _rootDirCluster & 0xFFFF;
   dir[1].file_size = 0;
 
   // Write the directory entries to the first sector
   if (!writeBlock(dirSector, buffer)) {
     _lastError = SD_ERROR_DISK_WRITE;
     return false;
   }
 
   // Create the directory entry in the root directory
   if (!createDirEntry(_rootDirCluster, dirname, SD_ATTR_DIRECTORY, dirCluster, 0)) {
     _lastError = SD_ERROR_DISK_WRITE;
     return false;
   }
 
   return true;
 }
 
 // Create a directory path (nested directories)
 bool ZBSdCard::createDirectoryPath(const char *path) {
   char pathCopy[100];
   strncpy(pathCopy, path, 99);
   pathCopy[99] = 0;
 
   // Remove trailing slash if present
   int len = strlen(pathCopy);
   if (len > 0 && pathCopy[len - 1] == '/') {
     pathCopy[len - 1] = 0;
   }
 
   // Remove leading slash if present
   char *currentPath = pathCopy;
   if (currentPath[0] == '/') {
     currentPath++;
   }
 
   // Start from root directory
   uint32_t parentCluster = _rootDirCluster;
 
   char *segment, *rest = currentPath;
   char segmentCopy[13];  // For 8.3 names
 
   // Process each directory in the path
   while ((segment = strtok_r(rest, "/", &rest))) {
     // Make a copy of the segment for safe keeping
     strncpy(segmentCopy, segment, 12);
     segmentCopy[12] = 0;
 
     // Check if this segment already exists
     DirectoryEntry dirEntry;
     if (!findDirEntry(parentCluster, segmentCopy, &dirEntry)) {
       // Need to create this directory level
       uint32_t newDirCluster = findFreeCluster();
       if (newDirCluster == 0) {
         _lastError = SD_ERROR_DISK_WRITE;
         return false;
       }
 
       // Mark the new cluster as end of chain
       writeFATEntry(newDirCluster, 0x0FFFFFFF);
 
       // Initialize with "." and ".." entries
       uint32_t dirSector = clusterToSector(newDirCluster);
 
       // Clear the new cluster
       memset(buffer, 0, ZBSD_BUFFER_SIZE);
       for (uint32_t i = 0; i < _sectorsPerCluster; i++) {
         if (!writeBlock(dirSector + i, buffer)) {
           _lastError = SD_ERROR_DISK_WRITE;
           return false;
         }
       }
 
       // Create "." and ".." entries
       memset(buffer, 0, ZBSD_BUFFER_SIZE);
       DirectoryEntry *dir = (DirectoryEntry *)buffer;
 
       // "." entry
       memset(dir[0].name, ' ', 11);
       dir[0].name[0] = '.';
       dir[0].attributes = SD_ATTR_DIRECTORY;
 
       // Set time/date
       uint16_t date = (2023 - 1980) << 9 | 5 << 5 | 2;
       uint16_t time = 12 << 11 | 0 << 5 | 0 / 2;
 
       dir[0].create_date = date;
       dir[0].create_time = time;
       dir[0].create_time_tenth = 0;
       dir[0].write_date = date;
       dir[0].write_time = time;
       dir[0].last_access_date = date;
 
       dir[0].first_cluster_high = (newDirCluster >> 16) & 0xFFFF;
       dir[0].first_cluster_low = newDirCluster & 0xFFFF;
       dir[0].file_size = 0;
 
       // ".." entry (points to parent)
       memset(dir[1].name, ' ', 11);
       dir[1].name[0] = '.';
       dir[1].name[1] = '.';
       dir[1].attributes = SD_ATTR_DIRECTORY;
 
       dir[1].create_date = date;
       dir[1].create_time = time;
       dir[1].create_time_tenth = 0;
       dir[1].write_date = date;
       dir[1].write_time = time;
       dir[1].last_access_date = date;
 
       dir[1].first_cluster_high = (parentCluster >> 16) & 0xFFFF;
       dir[1].first_cluster_low = parentCluster & 0xFFFF;
       dir[1].file_size = 0;
 
       // Write dir entries
       if (!writeBlock(dirSector, buffer)) {
         _lastError = SD_ERROR_DISK_WRITE;
         return false;
       }
 
       // Create the directory entry in the parent
       if (!createDirEntry(parentCluster, segmentCopy, SD_ATTR_DIRECTORY, newDirCluster, 0)) {
         _lastError = SD_ERROR_DISK_WRITE;
         return false;
       }
 
       // Update for next segment
       parentCluster = newDirCluster;
     } else {
       // Directory exists, check if it's really a directory
       if (!(dirEntry.attributes & SD_ATTR_DIRECTORY)) {
         _lastError = SD_ERROR_NO_PATH;
         return false;
       }
 
       // Move to the next level
       parentCluster = ((uint32_t)dirEntry.first_cluster_high << 16) | dirEntry.first_cluster_low;
     }
   }
 
   return true;
 }
 
 // Create a file in a specified directory path
 bool ZBSdCard::createFileInPath(const char *path, const char *content) {
   _lastError = SD_SUCCESS;
   
   // Parse the path to extract directory name and file name
   char pathCopy[100];
   strncpy(pathCopy, path, 99);
   pathCopy[99] = 0;
 
   // Find the last slash to separate directory and filename
   char *lastSlash = strrchr(pathCopy, '/');
   if (!lastSlash) {
     // No directory specified, create in root
     return createFile(path, content);
   }
 
   // Null-terminate to separate directory path and filename
   *lastSlash = 0;
   char *dirPath = pathCopy;
   char *fileName = lastSlash + 1;
 
   // Make sure the directory path exists
   if (strlen(dirPath) == 0) {
     // Root directory
     dirPath = "/";
   }
 
   // Find or create the directory structure
   uint32_t dirCluster;
   if (dirPath[0] == '/') {
     // Check if the directory exists
     dirCluster = findDirectoryByPath(dirPath);
     if (dirCluster == 0) {
       // Need to create the directory structure
       if (!createDirectoryPath(dirPath)) {
         return false;
       }
       dirCluster = findDirectoryByPath(dirPath);
       if (dirCluster == 0) {
         return false;
       }
     }
   } else {
     // Check if the directory exists
     dirCluster = findDirectoryByPath(dirPath);
     if (dirCluster == 0) {
       // Need to create the directory structure
       if (!createDirectoryPath(dirPath)) {
         return false;
       }
       dirCluster = findDirectoryByPath(dirPath);
       if (dirCluster == 0) {
         return false;
       }
     }
   }
 
   // Check if file exists in this directory
   DirectoryEntry fileEntry;
   uint32_t existingCluster = findDirEntry(dirCluster, fileName, &fileEntry);
 
   if (existingCluster != 0) {
     // File exists, overwrite
     uint32_t size = strlen(content);
     fileEntry.file_size = size;
     
     // Update directory entry
     uint32_t entryPos = findDirEntry(dirCluster, fileName, nullptr);
     if (entryPos == 0) {
       _lastError = SD_ERROR_NO_FILE;
       return false;
     }
     
     if (!writeDirEntry(entryPos, &fileEntry)) {
       _lastError = SD_ERROR_DISK_WRITE;
       return false;
     }
     
     // Get the file cluster
     uint32_t fileCluster = ((uint32_t)fileEntry.first_cluster_high << 16) | fileEntry.first_cluster_low;
     
     // Write content
     if (!writeFile(fileCluster, content, size, false)) {
       _lastError = SD_ERROR_DISK_WRITE;
       return false;
     }
     
     return true;
   } else {
     // Create new file in the specified directory
     uint32_t size = strlen(content);
 
     // Allocate cluster for the file
     uint32_t fileCluster = findFreeCluster();
     if (fileCluster == 0) {
       _lastError = SD_ERROR_DISK_WRITE;
       return false;
     }
 
     // Mark the cluster as end of chain
     writeFATEntry(fileCluster, 0x0FFFFFFF);
 
     // Create directory entry
     if (!createDirEntry(dirCluster, fileName, SD_ATTR_ARCHIVE, fileCluster, size)) {
       _lastError = SD_ERROR_DISK_WRITE;
       return false;
     }
 
     // Write content
     if (size > 0) {
       if (!writeFile(fileCluster, content, size, false)) {
         _lastError = SD_ERROR_DISK_WRITE;
         return false;
       }
     }
 
     return true;
   }
 }
#endif // ZBSD_MINIMAL_MODE
#endif // ZBSD_MINIMAL_MODE

 #ifdef BOARD_ZEROBASE
 /**
 * ZBSdCard - Minimal FAT32 for microcontrollers
 * Memory-optimized with essential FAT32 functions
 */

#include "ZBSdCard.h"

// Constructor
ZBSdCard::ZBSdCard(uint8_t csPin) {
  _csPin = csPin;
}

// Initialize SD card and FAT32 filesystem
bool ZBSdCard::begin() {
  // Init SPI
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV128);

  // Init SD card
  pinMode(_csPin, OUTPUT);
  digitalWrite(_csPin, HIGH);
  delay(50);

  // Send dummy clocks with CS high
  for (int i = 0; i < 80; i++) {
    SPI.transfer(0xFF);
  }

  // Initialize SD card
  if (!sdInit()) return false;

  // Initialize FAT32
  if (!initFAT32()) return false;

  return true;
}

// Initialize SD card
bool ZBSdCard::sdInit() {
  // Reset card with CMD0
  digitalWrite(_csPin, LOW);

  // Send CMD0 (GO_IDLE_STATE)
  SPI.transfer(0x40);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x95);  // CRC

  // Wait for response
  uint8_t r1;
  int timeout = 100;
  do {
    r1 = SPI.transfer(0xFF);
    timeout--;
  } while ((r1 & 0x80) && timeout > 0);

  SPI.transfer(0xFF);
  digitalWrite(_csPin, HIGH);
  SPI.transfer(0xFF);

  if (r1 != 0x01) return false;

  // Initialize with ACMD41
  for (int i = 0; i < 1000; i++) {
    // Send CMD55
    digitalWrite(_csPin, LOW);
    SPI.transfer(0x77);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    SPI.transfer(0xFF);

    int j = 100;
    do {
      r1 = SPI.transfer(0xFF);
      j--;
    } while ((r1 & 0x80) && j > 0);

    digitalWrite(_csPin, HIGH);
    SPI.transfer(0xFF);

    // Send ACMD41
    digitalWrite(_csPin, LOW);
    SPI.transfer(0x69);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    SPI.transfer(0xFF);

    j = 100;
    do {
      r1 = SPI.transfer(0xFF);
      j--;
    } while ((r1 & 0x80) && j > 0);

    SPI.transfer(0xFF);
    digitalWrite(_csPin, HIGH);
    SPI.transfer(0xFF);

    if (r1 == 0x00) {
      SPI.setClockDivider(SPI_CLOCK_DIV64);
      return true;
    }

    delay(10);
  }

  return false;
}

// Initialize FAT32 filesystem
bool ZBSdCard::initFAT32() {
  // Read MBR
  if (!sdReadBlock(0)) return false;

  // Check signature
  if (buffer[510] != 0x55 || buffer[511] != 0xAA) return false;

  // Get partition start
  partitionStart = buffer[454] | (buffer[455] << 8) | (buffer[456] << 16) | (buffer[457] << 24);

  // Read VBR
  if (!sdReadBlock(partitionStart)) return false;

  // Check signature
  if (buffer[510] != 0x55 || buffer[511] != 0xAA) return false;

  // Get parameters
  sectorsPerCluster = buffer[13];
  uint16_t reservedSectors = buffer[14] | (buffer[15] << 8);
  numFATs = buffer[16];
  fatSize = buffer[36] | (buffer[37] << 8) | (buffer[38] << 16) | (buffer[39] << 24);
  rootDirCluster = buffer[44] | (buffer[45] << 8) | (buffer[46] << 16) | (buffer[47] << 24);

  // Calculate sectors
  fatStartSector = partitionStart + reservedSectors;
  dataStartSector = fatStartSector + (numFATs * fatSize);

  return true;
}

// Read a sector
bool ZBSdCard::sdReadBlock(uint32_t blockAddr) {
  digitalWrite(_csPin, LOW);

  // Send CMD17 (READ_SINGLE_BLOCK)
  SPI.transfer(0x51);
  SPI.transfer((blockAddr >> 24) & 0xFF);
  SPI.transfer((blockAddr >> 16) & 0xFF);
  SPI.transfer((blockAddr >> 8) & 0xFF);
  SPI.transfer(blockAddr & 0xFF);
  SPI.transfer(0xFF);

  // Wait for response
  uint8_t r1;
  int timeout = 100;
  do {
    r1 = SPI.transfer(0xFF);
    timeout--;
  } while ((r1 & 0x80) && timeout > 0);

  if (r1 != 0x00) {
    digitalWrite(_csPin, HIGH);
    return false;
  }

  // Wait for data token
  timeout = 50000;
  uint8_t token;
  do {
    token = SPI.transfer(0xFF);
    timeout--;
  } while (token != 0xFE && timeout > 0);

  if (token != 0xFE) {
    digitalWrite(_csPin, HIGH);
    return false;
  }

  // Read data
  for (int i = 0; i < SECTOR_SIZE; i++) {
    buffer[i] = SPI.transfer(0xFF);
  }

  // Skip CRC
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);

  digitalWrite(_csPin, HIGH);
  SPI.transfer(0xFF);

  return true;
}

// Write a sector
bool ZBSdCard::sdWriteBlock(uint32_t blockAddr) {
  digitalWrite(_csPin, LOW);

  // Send CMD24 (WRITE_BLOCK)
  SPI.transfer(0x58);
  SPI.transfer((blockAddr >> 24) & 0xFF);
  SPI.transfer((blockAddr >> 16) & 0xFF);
  SPI.transfer((blockAddr >> 8) & 0xFF);
  SPI.transfer(blockAddr & 0xFF);
  SPI.transfer(0xFF);

  // Wait for response
  uint8_t r1;
  int timeout = 100;
  do {
    r1 = SPI.transfer(0xFF);
    timeout--;
  } while ((r1 & 0x80) && timeout > 0);

  if (r1 != 0x00) {
    digitalWrite(_csPin, HIGH);
    return false;
  }

  // Send data token
  SPI.transfer(0xFF);
  SPI.transfer(0xFE);

  // Send data
  for (int i = 0; i < SECTOR_SIZE; i++) {
    SPI.transfer(buffer[i]);
  }

  // Send dummy CRC
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);

  // Get data response
  r1 = SPI.transfer(0xFF);

  if ((r1 & 0x1F) != 0x05) {
    digitalWrite(_csPin, HIGH);
    return false;
  }

  // Wait for write to complete
  timeout = 50000;
  while (SPI.transfer(0xFF) == 0x00 && timeout > 0) {
    timeout--;
  }

  digitalWrite(_csPin, HIGH);
  SPI.transfer(0xFF);

  return (timeout > 0);
}

// Convert cluster to sector
uint32_t ZBSdCard::clusterToSector(uint32_t cluster) {
  return dataStartSector + ((cluster - 2) * sectorsPerCluster);
}

// Get a FAT entry
uint32_t ZBSdCard::getFatEntry(uint32_t cluster) {
  uint32_t fatOffset = cluster * 4;
  uint32_t fatSector = fatStartSector + (fatOffset / SECTOR_SIZE);
  uint32_t offset = fatOffset % SECTOR_SIZE;

  if (!sdReadBlock(fatSector)) return FAT_EOC;

  uint32_t value = buffer[offset] | (buffer[offset + 1] << 8) | (buffer[offset + 2] << 16) | (buffer[offset + 3] << 24);

  return value & 0x0FFFFFFF;
}

// Set a FAT entry
bool ZBSdCard::setFatEntry(uint32_t cluster, uint32_t value) {
  for (uint8_t fatNum = 0; fatNum < numFATs; fatNum++) {
    uint32_t fatOffset = cluster * 4;
    uint32_t fatSector = fatStartSector + (fatNum * fatSize) + (fatOffset / SECTOR_SIZE);
    uint32_t offset = fatOffset % SECTOR_SIZE;

    if (!sdReadBlock(fatSector)) return false;

    // Preserve top 4 bits
    uint32_t oldValue = buffer[offset] | (buffer[offset + 1] << 8) | (buffer[offset + 2] << 16) | (buffer[offset + 3] << 24);

    uint32_t newValue = (oldValue & 0xF0000000) | (value & 0x0FFFFFFF);

    buffer[offset] = newValue & 0xFF;
    buffer[offset + 1] = (newValue >> 8) & 0xFF;
    buffer[offset + 2] = (newValue >> 16) & 0xFF;
    buffer[offset + 3] = (newValue >> 24) & 0xFF;

    if (!sdWriteBlock(fatSector)) return false;
  }

  return true;
}

// Find a free cluster
uint32_t ZBSdCard::findFreeCluster() {
  for (uint32_t cluster = 2; cluster < 100; cluster++) {
    if (getFatEntry(cluster) == 0) return cluster;
  }
  return 0;
}

// Convert filename to FAT 8.3 format
void ZBSdCard::filenameToFAT83(const char* filename, char* fatName) {
  memset(fatName, ' ', 11);

  // Handle special directory cases
  if (strcmp(filename, ".") == 0) {
    fatName[0] = '.';
    return;
  }

  if (strcmp(filename, "..") == 0) {
    fatName[0] = '.';
    fatName[1] = '.';
    return;
  }

  // Find path separator
  const char* lastSlash = strrchr(filename, '/');
  const char* name = lastSlash ? lastSlash + 1 : filename;

  // Find extension
  const char* dot = strchr(name, '.');
  int nameLen = dot ? (dot - name) : strlen(name);
  if (nameLen > 8) nameLen = 8;

  // Copy name
  for (int i = 0; i < nameLen; i++) {
    fatName[i] = toupper(name[i]);
  }

  // Copy extension
  if (dot) {
    int extPos = 8;
    for (int i = 0; i < 3 && dot[i + 1]; i++) {
      fatName[extPos++] = toupper(dot[i + 1]);
    }
  }
}

// =============== NEW HELPER FUNCTION ===============
// Find an entry within a specific directory cluster
uint16_t ZBSdCard::findEntryInDir(uint32_t dirCluster, const char* name, uint32_t* cluster, uint8_t* attr) {
  char fatName[11];
  filenameToFAT83(name, fatName);
  
  uint32_t sector = clusterToSector(dirCluster);
  if (!sdReadBlock(sector)) return 0xFFFF;
  
  // Scan entries
  for (uint16_t i = 0; i < SECTOR_SIZE; i += DIR_ENTRY_SIZE) {
    // End of directory
    if (buffer[i] == 0) break;
    
    // Skip deleted entries
    if (buffer[i] == 0xE5) continue;
    
    // Skip LFN entries
    if (buffer[i + 11] == ATTR_LONG_NAME) continue;
    
    // Compare name
    if (memcmp(buffer + i, fatName, 11) == 0) {
      // Get cluster and attributes
      if (cluster) {
        uint32_t high = buffer[i + 20] | (buffer[i + 21] << 8);
        uint32_t low = buffer[i + 26] | (buffer[i + 27] << 8);
        *cluster = (high << 16) | low;
      }
      
      if (attr) {
        *attr = buffer[i + 11];
      }
      
      return i; // Return offset in directory
    }
  }
  
  return 0xFFFF; // Not found
}

/**
 * Enhanced ZBSdCard Library
 * Improved path handling for deeper directory nesting
 * 
 * This modifies the key functions that handle path navigation
 */

// =============== REPLACEMENT FOR findDirEntry FUNCTION ===============
uint16_t ZBSdCard::findDirEntry(const char* name, uint32_t* cluster, uint8_t* attr) {
  // Start at root directory
  uint32_t dirCluster = rootDirCluster;
  
  // Make a copy of the path to work with
  char pathCopy[80];
  strncpy(pathCopy, name, sizeof(pathCopy) - 1);
  pathCopy[sizeof(pathCopy) - 1] = 0; // Ensure null termination
  
  char* path = pathCopy;
  
  // Skip any leading slash
  if (path[0] == '/') path++;
  
  // Process all path segments except the last one
  char* segment = NULL;
  char* nextSegment = path;
  char* lastSlash = strrchr(path, '/');
  
  // If there's no slash, we're looking for something in the root directory
  if (!lastSlash) {
    // Handle file/dir in root directory
    return findEntryInDir(dirCluster, path, cluster, attr);
  }
  
  // Process each directory segment
  while (nextSegment && nextSegment < lastSlash) {
    // Find the next segment
    segment = nextSegment;
    nextSegment = strchr(segment, '/');
    
    if (nextSegment) {
      // Temporarily null-terminate this segment
      *nextSegment = 0;
      
      // Find this segment in the current directory
      uint32_t nextCluster;
      uint8_t entryAttr;
      if (findEntryInDir(dirCluster, segment, &nextCluster, &entryAttr) == 0xFFFF) {
        return 0xFFFF; // Segment not found
      }
      
      // Verify it's a directory
      if (!(entryAttr & ATTR_DIRECTORY)) {
        return 0xFFFF; // Not a directory
      }
      
      // Advance to next directory
      dirCluster = nextCluster;
      
      // Restore the path separator and move to next segment
      *nextSegment = '/';
      nextSegment++;
    }
  }
  
  // At this point, lastSlash+1 is the filename or directory name to find
  return findEntryInDir(dirCluster, lastSlash + 1, cluster, attr);
}

// Create a file
bool ZBSdCard::createFile(const char* filename, const char* data, uint16_t len) {
  // Delete if file already exists
  if (fileExists(filename)) {
    deleteFile(filename);
  }

  // Parse path
  const char* lastSlash = strrchr(filename, '/');
  const char* fname = lastSlash ? lastSlash + 1 : filename;
  uint32_t dirCluster = rootDirCluster;

  // If there's a path, navigate to directory
  if (lastSlash) {
    char dirPath[64];
    strncpy(dirPath, filename, lastSlash - filename);
    dirPath[lastSlash - filename] = 0;

    // Find directory
    uint32_t tmpCluster;
    uint8_t tmpAttr;
    if (findDirEntry(dirPath, &tmpCluster, &tmpAttr) == 0xFFFF) {
      return false;
    }

    if (!(tmpAttr & ATTR_DIRECTORY)) {
      return false;
    }

    dirCluster = tmpCluster;
  }

  // Allocate a cluster for data
  uint32_t fileCluster = findFreeCluster();
  if (fileCluster == 0) return false;

  // Mark as end-of-chain
  if (!setFatEntry(fileCluster, FAT_EOC)) return false;

  // Read directory sector
  uint32_t dirSector = clusterToSector(dirCluster);
  if (!sdReadBlock(dirSector)) return false;

  // Find a free entry
  uint16_t entryOffset = 0xFFFF;
  for (uint16_t i = 0; i < SECTOR_SIZE; i += DIR_ENTRY_SIZE) {
    if (buffer[i] == 0 || buffer[i] == 0xE5) {
      entryOffset = i;
      break;
    }
  }

  if (entryOffset == 0xFFFF) return false;

  // Create directory entry
  char fatName[11];
  filenameToFAT83(fname, fatName);

  memset(buffer + entryOffset, 0, DIR_ENTRY_SIZE);
  memcpy(buffer + entryOffset, fatName, 11);

  // Set attributes (archive)
  buffer[entryOffset + 11] = ATTR_ARCHIVE;

  // Set cluster
  buffer[entryOffset + 20] = (fileCluster >> 16) & 0xFF;
  buffer[entryOffset + 21] = (fileCluster >> 24) & 0xFF;
  buffer[entryOffset + 26] = fileCluster & 0xFF;
  buffer[entryOffset + 27] = (fileCluster >> 8) & 0xFF;

  // Set size
  buffer[entryOffset + 28] = len & 0xFF;
  buffer[entryOffset + 29] = (len >> 8) & 0xFF;
  buffer[entryOffset + 30] = (len >> 16) & 0xFF;
  buffer[entryOffset + 31] = (len >> 24) & 0xFF;

  // Write directory entry
  if (!sdWriteBlock(dirSector)) return false;

  // Write file data
  uint32_t dataSector = clusterToSector(fileCluster);
  memset(buffer, 0, SECTOR_SIZE);
  memcpy(buffer, data, len);

  return sdWriteBlock(dataSector);
}

// Read a file
bool ZBSdCard::readFile(const char* filename, char* data, uint16_t maxLen, uint16_t* bytesRead) {
  uint32_t fileCluster;
  uint8_t attr;

  // Find the file
  if (findDirEntry(filename, &fileCluster, &attr) == 0xFFFF) {
    return false;
  }

  // Check if it's a file
  if (attr & ATTR_DIRECTORY) {
    return false;
  }

  // Read the data sector
  uint32_t dataSector = clusterToSector(fileCluster);
  if (!sdReadBlock(dataSector)) {
    return false;
  }

  // Determine how much to read (limited by buffer size)
  uint16_t toRead = (maxLen < SECTOR_SIZE) ? maxLen : SECTOR_SIZE;

  // Copy data
  memcpy(data, buffer, toRead);
  data[toRead] = 0;  // Null terminate

  if (bytesRead) {
    *bytesRead = toRead;
  }

  return true;
}

// Check if a file exists
bool ZBSdCard::fileExists(const char* filename) {
  uint32_t cluster;
  uint8_t attr;

  if (findDirEntry(filename, &cluster, &attr) == 0xFFFF) {
    return false;
  }

  return !(attr & ATTR_DIRECTORY);
}

// Check if a directory exists
bool ZBSdCard::dirExists(const char* dirname) {
  uint32_t cluster;
  uint8_t attr;

  if (findDirEntry(dirname, &cluster, &attr) == 0xFFFF) {
    return false;
  }

  return (attr & ATTR_DIRECTORY);
}

// =============== NEW HELPER FUNCTION ===============
// Internal function to create a directory in a specific parent
bool ZBSdCard::createDirInternal(const char* dirname, uint32_t parentCluster) {
  // Allocate a cluster for the directory
  uint32_t dirCluster = findFreeCluster();
  if (dirCluster == 0) return false;
  
  // Mark as end-of-chain
  if (!setFatEntry(dirCluster, FAT_EOC)) return false;
  
  // Initialize with "." and ".." entries
  memset(buffer, 0, SECTOR_SIZE);
  
  // "." entry
  buffer[0] = '.';
  memset(buffer + 1, ' ', 10);
  buffer[11] = ATTR_DIRECTORY;
  buffer[20] = (dirCluster >> 16) & 0xFF;
  buffer[21] = (dirCluster >> 24) & 0xFF;
  buffer[26] = dirCluster & 0xFF;
  buffer[27] = (dirCluster >> 8) & 0xFF;
  
  // ".." entry
  buffer[32] = '.';
  buffer[33] = '.';
  memset(buffer + 34, ' ', 9);
  buffer[43] = ATTR_DIRECTORY;
  
  // Set parent cluster in the ".." entry
  buffer[52] = (parentCluster >> 16) & 0xFF;
  buffer[53] = (parentCluster >> 24) & 0xFF;
  buffer[58] = parentCluster & 0xFF;
  buffer[59] = (parentCluster >> 8) & 0xFF;
  
  // Write the directory cluster
  uint32_t dirSector = clusterToSector(dirCluster);
  if (!sdWriteBlock(dirSector)) return false;
  
  // Create entry in parent directory
  char fatName[11];
  filenameToFAT83(dirname, fatName);
  
  uint32_t parentSector = clusterToSector(parentCluster);
  if (!sdReadBlock(parentSector)) return false;
  
  // Find a free entry
  uint16_t entryOffset = 0xFFFF;
  for (uint16_t i = 0; i < SECTOR_SIZE; i += DIR_ENTRY_SIZE) {
    if (buffer[i] == 0 || buffer[i] == 0xE5) {
      entryOffset = i;
      break;
    }
  }
  
  if (entryOffset == 0xFFFF) return false;
  
  // Create directory entry
  memset(buffer + entryOffset, 0, DIR_ENTRY_SIZE);
  memcpy(buffer + entryOffset, fatName, 11);
  
  // Set attributes
  buffer[entryOffset + 11] = ATTR_DIRECTORY;
  
  // Set cluster
  buffer[entryOffset + 20] = (dirCluster >> 16) & 0xFF;
  buffer[entryOffset + 21] = (dirCluster >> 24) & 0xFF;
  buffer[entryOffset + 26] = dirCluster & 0xFF;
  buffer[entryOffset + 27] = (dirCluster >> 8) & 0xFF;
  
  // Write parent directory
  return sdWriteBlock(parentSector);
}

// =============== REPLACEMENT FOR createDirectory FUNCTION ===============
bool ZBSdCard::createDirectory(const char* dirname) {
  // Return true if directory already exists
  if (dirExists(dirname)) {
    return true;
  }
  
  // Parse path to find parent directory
  char pathCopy[80];
  strncpy(pathCopy, dirname, sizeof(pathCopy) - 1);
  pathCopy[sizeof(pathCopy) - 1] = 0; // Ensure null termination
  
  char* lastSlash = strrchr(pathCopy, '/');
  if (!lastSlash) {
    // This is a root-level directory
    return createDirInternal(dirname, rootDirCluster);
  }
  
  // Temporarily terminate the parent path
  *lastSlash = 0;
  const char* dirName = lastSlash + 1;
  
  // Make sure parent directory exists - recursive call
  if (!createDirectory(pathCopy)) {
    return false;
  }
  
  // Find the parent directory cluster
  uint32_t parentCluster;
  uint8_t attr;
  if (findDirEntry(pathCopy, &parentCluster, &attr) == 0xFFFF) {
    return false;
  }
  
  if (!(attr & ATTR_DIRECTORY)) {
    return false; // Parent is not a directory
  }
  
  // Now create the actual directory
  return createDirInternal(dirName, parentCluster);
}


// Delete a file
bool ZBSdCard::deleteFile(const char* filename) {
  uint32_t fileCluster;
  uint8_t attr;

  // Find the file
  const char* lastSlash = strrchr(filename, '/');
  const char* fname = lastSlash ? lastSlash + 1 : filename;
  uint32_t dirCluster = rootDirCluster;

  // If there's a path, navigate to directory
  if (lastSlash) {
    char dirPath[64];
    strncpy(dirPath, filename, lastSlash - filename);
    dirPath[lastSlash - filename] = 0;

    // Find directory
    uint32_t tmpCluster;
    uint8_t tmpAttr;
    if (findDirEntry(dirPath, &tmpCluster, &tmpAttr) == 0xFFFF) {
      return false;
    }

    if (!(tmpAttr & ATTR_DIRECTORY)) {
      return false;
    }

    dirCluster = tmpCluster;
  }

  // Read directory sector
  uint32_t dirSector = clusterToSector(dirCluster);
  if (!sdReadBlock(dirSector)) return false;

  // Find the file entry
  char fatName[11];
  filenameToFAT83(fname, fatName);

  uint16_t entryOffset = 0xFFFF;
  for (uint16_t i = 0; i < SECTOR_SIZE; i += DIR_ENTRY_SIZE) {
    if (buffer[i] == 0) break;

    if (memcmp(buffer + i, fatName, 11) == 0) {
      entryOffset = i;

      // Get cluster number
      uint32_t high = buffer[i + 20] | (buffer[i + 21] << 8);
      uint32_t low = buffer[i + 26] | (buffer[i + 27] << 8);
      fileCluster = (high << 16) | low;

      attr = buffer[i + 11];
      break;
    }
  }

  if (entryOffset == 0xFFFF) return false;

  // Check if it's a file
  if (attr & ATTR_DIRECTORY) return false;

  // Mark entry as deleted
  buffer[entryOffset] = 0xE5;

  // Write directory sector
  if (!sdWriteBlock(dirSector)) return false;

  // Free cluster chain
  uint32_t nextCluster;
  while (fileCluster >= 2 && fileCluster < FAT_EOC) {
    nextCluster = getFatEntry(fileCluster);
    if (!setFatEntry(fileCluster, 0)) return false;
    fileCluster = nextCluster;
  }

  return true;
}

// Delete a directory
bool ZBSdCard::deleteDirectory(const char* dirname) {
  uint32_t dirCluster;
  uint8_t attr;

  // Find the directory
  if (findDirEntry(dirname, &dirCluster, &attr) == 0xFFFF) {
    return false;
  }

  // Check if it's a directory
  if (!(attr & ATTR_DIRECTORY)) {
    return false;
  }

  // Check if it's empty (read the directory cluster)
  uint32_t sector = clusterToSector(dirCluster);
  if (!sdReadBlock(sector)) return false;

  // Check for entries beyond "." and ".."
  for (uint16_t i = 2 * DIR_ENTRY_SIZE; i < SECTOR_SIZE; i += DIR_ENTRY_SIZE) {
    if (buffer[i] == 0) break;

    if (buffer[i] != 0xE5) {
      return false;  // Directory not empty
    }
  }

  // Directory is empty, delete entry from parent directory
  const char* lastSlash = strrchr(dirname, '/');
  const char* dirName = lastSlash ? lastSlash + 1 : dirname;
  uint32_t parentCluster = rootDirCluster;

  // If there's a path, navigate to parent directory
  if (lastSlash) {
    char parentPath[64];
    strncpy(parentPath, dirname, lastSlash - dirname);
    parentPath[lastSlash - dirname] = 0;

    // Find parent directory
    uint32_t tmpCluster;
    uint8_t tmpAttr;
    if (findDirEntry(parentPath, &tmpCluster, &tmpAttr) == 0xFFFF) {
      return false;
    }

    if (!(tmpAttr & ATTR_DIRECTORY)) {
      return false;
    }

    parentCluster = tmpCluster;
  }

  // Read parent directory sector
  uint32_t parentSector = clusterToSector(parentCluster);
  if (!sdReadBlock(parentSector)) return false;

  // Find the directory entry
  char fatName[11];
  filenameToFAT83(dirName, fatName);

  uint16_t entryOffset = 0xFFFF;
  for (uint16_t i = 0; i < SECTOR_SIZE; i += DIR_ENTRY_SIZE) {
    if (buffer[i] == 0) break;

    if (memcmp(buffer + i, fatName, 11) == 0) {
      entryOffset = i;
      break;
    }
  }

  if (entryOffset == 0xFFFF) return false;

  // Mark entry as deleted
  buffer[entryOffset] = 0xE5;

  // Write parent directory sector
  if (!sdWriteBlock(parentSector)) return false;

  // Free the directory cluster
  return setFatEntry(dirCluster, 0);
}

bool ZBSdCard::writeFile(const char* filename, const char* data) {
  // Handle null data case
  if (data == NULL) {
    // Serial1.println("Data is NULL");
    return createFile(filename, "", 0);
  }
  
  // Calculate string length
  uint16_t len = 0;
  while (data[len] != '\0' && len < SECTOR_SIZE) {
    len++;
  }
  // Call the original function with the calculated length
  bool result = createFile(filename, data, len);

  return result;
}
 #endif
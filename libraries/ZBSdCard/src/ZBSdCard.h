/**
 * ZBSdCard.h - Highly optimized library for SD card operations on CH32V003/CH32V203
 * Dual-platform support with minimized RAM and flash footprint
 */

#ifdef BOARD_ZEROBASE2


 #ifndef ZBSdCard_h
 #define ZBSdCard_h
 
#include <Arduino.h>
 #include <SPI.h>
 // Determine target platform
 #if defined(BOARD_ZEROBASE)
   #define ZBSD_MINIMAL_MODE  // Enable minimal mode for CH32V003
   #define ZBSD_BUFFER_SIZE 256  // Smaller buffer for CH32V003
 #else
   #define ZBSD_BUFFER_SIZE 512  // Full buffer for CH32V203
 #endif
 
 // SD card commands
 #define SD_CMD0     0     // GO_IDLE_STATE - reset card to idle state
 #define SD_CMD1     1     // SEND_OP_COND - initialize card
 #define SD_CMD8     8     // SEND_IF_COND - verify SD card interface operating condition
 #define SD_CMD9     9     // SEND_CSD - read Card Specific Data
 #define SD_CMD12    12    // STOP_TRANSMISSION - end multiple block read
 #define SD_CMD13    13    // SEND_STATUS - read card status register
 #define SD_CMD16    16    // SET_BLOCKLEN - set the block length
 #define SD_CMD17    17    // READ_SINGLE_BLOCK - read a single data block
 #define SD_CMD18    18    // READ_MULTIPLE_BLOCK - read multiple data blocks
 #define SD_CMD24    24    // WRITE_BLOCK - write a single data block
 #define SD_CMD25    25    // WRITE_MULTIPLE_BLOCK - write multiple data blocks
 #define SD_CMD32    32    // ERASE_WR_BLK_START - sets the address of first block to erase
 #define SD_CMD33    33    // ERASE_WR_BLK_END - sets the address of last block to erase
 #define SD_CMD38    38    // ERASE - erase all previously selected blocks
 #define SD_CMD55    55    // APP_CMD - next command is application specific
 #define SD_CMD58    58    // READ_OCR - read the OCR register
 #define SD_ACMD41   41    // SD_SEND_OP_COND - sends host capacity support information
 
 // Response codes
 #define SD_R1_READY_STATE  0x00
 #define SD_R1_IDLE_STATE   0x01
 #define SD_R1_ILLEGAL_CMD  0x04
 
 // Data tokens
 #define SD_DATA_START_BLOCK  0xFE   // Start block token for single block read/write
 #define SD_DATA_RES_MASK     0x1F   // Mask for data response token after write
 #define SD_DATA_RES_ACCEPTED 0x05   // Data accepted token
 
 // FAT32 Attributes
 #define SD_ATTR_READ_ONLY   0x01
 #define SD_ATTR_HIDDEN      0x02
 #define SD_ATTR_SYSTEM      0x04
 #define SD_ATTR_VOLUME_ID   0x08
 #define SD_ATTR_DIRECTORY   0x10
 #define SD_ATTR_ARCHIVE     0x20
 #define SD_ATTR_LONG_NAME   (SD_ATTR_READ_ONLY | SD_ATTR_HIDDEN | SD_ATTR_SYSTEM | SD_ATTR_VOLUME_ID)
 
 // Status and error codes
 #define SD_SUCCESS           0  // Operation successful
 #define SD_ERROR_INIT        1  // Initialization error
 #define SD_ERROR_DISK_READ   2  // Read error
 #define SD_ERROR_DISK_WRITE  3  // Write error
 #define SD_ERROR_FAT_READ    4  // FAT read error
 #define SD_ERROR_NO_FILE     5  // File not found
 #define SD_ERROR_NO_PATH     6  // Path not found
 
 class ZBSdCard {
 private:
     // Block size for SD card operations (512 is physical size)
     static const int BLOCK_SIZE = 512;
     
     // Buffer for data - reduce size for minimal mode
     uint8_t buffer[ZBSD_BUFFER_SIZE];
     
     // Card and filesystem variables
     uint8_t _csPin;                 // Chip select pin
     SPIClass* _spi;                 // SPI interface pointer
     bool _isSDHC;                   // Flag for SDHC card type
     uint32_t _partitionStartSector; // Start sector of the FAT32 partition
     uint32_t _fatStartSector;       // Start sector of the FAT table
     uint32_t _rootDirCluster;       // Cluster number of the root directory
     uint32_t _dataStartSector;      // Start sector of the data area
     uint8_t _sectorsPerCluster;     // Number of sectors per cluster
     uint32_t _clusterSize;          // Size of a cluster in bytes
     uint32_t _firstFreeFAT;         // First free cluster to use
     uint8_t _lastError;             // Last error code
     
     // FAT32 structures - made more compact to save RAM
     typedef struct {
       uint8_t status;
       uint8_t first_chs[3];
       uint8_t type;
       uint8_t last_chs[3];
       uint32_t first_sector;
       uint32_t sector_count;
     } __attribute__((packed)) PartitionEntry;
 
     typedef struct {
       uint8_t jump_boot[3];
       char oem_name[8];
       uint16_t bytes_per_sector;
       uint8_t sectors_per_cluster;
       uint16_t reserved_sector_count;
       uint8_t fat_count;
       uint16_t root_entry_count;
       uint16_t total_sectors_16;
       uint8_t media_type;
       uint16_t fat_size_16;
       uint16_t sectors_per_track;
       uint16_t head_count;
       uint32_t hidden_sectors;
       uint32_t total_sectors_32;
       uint32_t fat_size_32;
       uint16_t ext_flags;
       uint16_t fs_version;
       uint32_t root_cluster;
       uint16_t fs_info;
       uint16_t backup_boot_sector;
       uint8_t reserved[12];
       uint8_t drive_number;
       uint8_t reserved1;
       uint8_t boot_signature;
       uint32_t volume_id;
       char volume_label[11];
       char fs_type[8];
     } __attribute__((packed)) FAT32BootSector;
 
     typedef struct {
       char name[8];
       char ext[3];
       uint8_t attributes;
       uint8_t reserved;
       uint8_t create_time_tenth;
       uint16_t create_time;
       uint16_t create_date;
       uint16_t last_access_date;
       uint16_t first_cluster_high;
       uint16_t write_time;
       uint16_t write_date;
       uint16_t first_cluster_low;
       uint32_t file_size;
     } __attribute__((packed)) DirectoryEntry;
     
     // Private methods
     bool initializeSD();
     uint8_t sendCommand(uint8_t cmd, uint32_t arg);
     uint8_t sendAppCommand(uint8_t cmd, uint32_t arg);
     bool readBlock(uint32_t blockAddr, uint8_t *data);
     bool writeBlock(uint32_t blockAddr, const uint8_t *data);
     bool findPartition();
     bool readFAT32BootSector();
     uint32_t clusterToSector(uint32_t cluster);
     uint32_t readFATEntry(uint32_t cluster);
     bool writeFATEntry(uint32_t cluster, uint32_t value);
     uint32_t findFreeCluster();
     void filenameToFAT83(const char *filename, char *fatName);
     uint32_t findDirectoryByPath(const char *path);
     uint32_t findDirEntry(uint32_t dirCluster, const char *name, DirectoryEntry *entry);
     uint32_t findInRootDir(const char *name, DirectoryEntry *entry = nullptr);
     bool writeDirEntry(uint32_t entryPos, const DirectoryEntry *entry);
     uint32_t findFreeDirEntry(uint32_t dirCluster);
     uint32_t createDirEntry(uint32_t dirCluster, const char *name, uint8_t attributes, uint32_t fileCluster, uint32_t fileSize);
     bool writeFile(uint32_t cluster, const char *data, uint32_t size, bool append = false);
     uint32_t findFile(const char *filename, DirectoryEntry *entry = nullptr);
 
 public:
     ZBSdCard();
     
     // Basic initialization - modified to accept SPI interface
     bool begin(uint8_t csPin, SPIClass& spi = SPI);
     
     // Core file operations - available on all platforms
     bool createFile(const char *fileName, const char *content = "");
     uint32_t readFile(const char *fileName, char *buffer, uint32_t bufferSize);
     bool appendFile(const char *fileName, const char *content);
     bool deleteFile(const char *fileName);
 
     // Directory listing functions - common interface
     bool listDirectory(const char *path = "/");
     bool listDirectoryEntries(const char* path, char (*fileNames)[20], bool* isDirectory, int maxEntries, int* entryCount);
     
 #ifndef ZBSD_MINIMAL_MODE
     // Extended operations - only available on CH32V203
     bool createDirectory(const char *dirname);
     bool createDirectoryPath(const char *path);
     bool createFileInPath(const char *path, const char *content);
 #endif
 
     // Error handling - available on all platforms
     const char* getErrorMessage();
     uint8_t getLastError() { return _lastError; }
 };
 #endif // BOARD_ZEROBASE2
 #endif // ZBSdCard_h
 
#ifdef BOARD_ZEROBASE
/**
 * ZBSdCard - Minimal FAT32 for microcontrollers
 * Memory-optimized with essential FAT32 functions
 */

#ifndef ZBSDCARD_H
#define ZBSDCARD_H

#include <Arduino.h>
#include <SPI.h>
#include <string.h>

class ZBSdCard {
  private:
    uint8_t _csPin;
    uint8_t buffer[512];
    uint32_t partitionStart;
    uint32_t fatStartSector;
    uint32_t rootDirCluster;
    uint32_t dataStartSector;
    uint8_t sectorsPerCluster;
    uint8_t numFATs;
    uint32_t fatSize;
    
    // FAT32 constants
    static const uint16_t SECTOR_SIZE = 512;
    static const uint16_t DIR_ENTRY_SIZE = 32;
    static const uint8_t ATTR_DIRECTORY = 0x10;
    static const uint8_t ATTR_ARCHIVE = 0x20;
    static const uint8_t ATTR_LONG_NAME = 0x0F;
    static const uint32_t FAT_EOC = 0x0FFFFFFF;
    
    // Private methods
    bool sdInit();
    bool initFAT32();
    bool sdReadBlock(uint32_t blockAddr);
    bool sdWriteBlock(uint32_t blockAddr);
    uint32_t clusterToSector(uint32_t cluster);
    uint32_t getFatEntry(uint32_t cluster);
    bool setFatEntry(uint32_t cluster, uint32_t value);
    uint32_t findFreeCluster();
    void filenameToFAT83(const char* filename, char* fatName);
    uint16_t findDirEntry(const char* name, uint32_t* cluster, uint8_t* attr);
    uint16_t findEntryInDir(uint32_t dirCluster, const char* name, uint32_t* cluster, uint8_t* attr);
  bool createDirInternal(const char* dirname, uint32_t parentCluster);
  public:
    ZBSdCard(uint8_t csPin);
    bool begin();
    
    // Original function with explicit length
    bool createFile(const char* filename, const char* data, uint16_t len);
    
  /**
 * Alternate version of createFile that calculates length automatically
 * Add this to your ZBSdCard.h in the public section:
 */
bool writeFile(const char* filename, const char* data);
    
    bool readFile(const char* filename, char* data, uint16_t maxLen, uint16_t* bytesRead);
    bool fileExists(const char* filename);
    bool createDirectory(const char* dirname);
    bool dirExists(const char* dirname);
    bool deleteFile(const char* filename);
    bool deleteDirectory(const char* dirname);
    
};

#endif
#endif // BOARD_ZEROBASE

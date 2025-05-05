// ZBCAN.h
#ifndef ZBCAN_H
#define ZBCAN_H
#include <Arduino.h>

/* Frame Format Definition */
#define ZBCAN_STANDARD_FRAME 0
#define ZBCAN_EXTENDED_FRAME 1

/* Buffer definitions */
#define ZBCAN_MAX_BUFFER_SIZE 128
#define ZBCAN_MAX_FRAME_SIZE 8
#define ZBCAN_MAX_EXPECTED_FRAMES 32

/* CAN Test Mode Definitions */
#define ZBCAN_MODE_NORMAL CAN_Mode_Normal
#define ZBCAN_MODE_LOOPBACK CAN_Mode_LoopBack
#define ZBCAN_MODE_SILENT CAN_Mode_Silent
#define ZBCAN_MODE_SILENT_LOOPBACK CAN_Mode_Silent_LoopBack

/* Message queue size - how many complete messages can be buffered */
#define ZBCAN_MESSAGE_QUEUE_SIZE 10

/* Message type definition (internal use) */
#define ZBCAN_TYPE_STRING 0 // Default for backward compatibility
#define ZBCAN_TYPE_INT8   1
#define ZBCAN_TYPE_UINT8  2
#define ZBCAN_TYPE_INT16  3
#define ZBCAN_TYPE_UINT16 4
#define ZBCAN_TYPE_INT32  5
#define ZBCAN_TYPE_UINT32 6
#define ZBCAN_TYPE_INT64  7
#define ZBCAN_TYPE_UINT64 8
#define ZBCAN_TYPE_RAW    9 // Raw binary data

// User-friendly message type definitions
#define ZBCAN_MSG_STRING  0  // String messages
#define ZBCAN_MSG_INT     1  // All integer types
#define ZBCAN_MSG_UINT    2  // All unsigned integer types
#define ZBCAN_MSG_LONG    3  // 64-bit integer
#define ZBCAN_MSG_ULONG   4  // 64-bit unsigned integer
#define ZBCAN_MSG_RAW     5  // Raw binary data

// Structure to hold a complete CAN message
typedef struct {
  uint8_t data[ZBCAN_MAX_BUFFER_SIZE + 1];  // +1 for null terminator
  size_t length;
  uint32_t id;
  uint8_t type;  // Message type (string, numeric, etc.)
  bool valid;
} ZBCanMessage;

// Callback types
typedef void (*ZBCanCallback)();  // Simple callback without parameters
typedef void (*ZBCanParamCallback)(uint32_t id, const uint8_t* data, size_t length, uint8_t type);
typedef void (*ZBCanStringCallback)(uint32_t id, const char* message, size_t length);
typedef void (*ZBCanIntCallback)(uint32_t id, int32_t value);
typedef void (*ZBCanUintCallback)(uint32_t id, uint32_t value);
typedef void (*ZBCanLongCallback)(uint32_t id, int64_t value);
typedef void (*ZBCanUlongCallback)(uint32_t id, uint64_t value);

class ZBCan
{
public:
  ZBCan();

  /**
   * @brief Initialize CAN communication with defaults
   * Defaults to 500kbps, normal mode, standard frames, 
   * and accepts all CAN IDs
   * 
   * @return true if initialization successful, false otherwise
   */
  bool begin();

  /**
   * @brief Initialize CAN communication with custom settings
   * 
   * @param baud Desired baudrate (e.g., 500000 for 500kbps)
   * @param mode Test mode
   * @param txId CAN ID to use for transmitting
   * @param frameFormat Frame format (ZBCAN_STANDARD_FRAME or ZBCAN_EXTENDED_FRAME)
   * @return true if initialization successful, false otherwise
   */
  bool begin(uint32_t baud, uint8_t mode = ZBCAN_MODE_NORMAL, 
             uint32_t txId = 0x100, uint8_t frameFormat = ZBCAN_STANDARD_FRAME);

  /*----- SENDING METHODS -----*/

  /**
   * @brief Sends a string message
   *
   * @param message String to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(const char *message, uint32_t id = 0);

  /**
   * @brief Sends a standard integer value
   * This handles regular int type without ambiguity
   *
   * @param value Value to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(int value, uint32_t id = 0);

  /**
   * @brief Sends an unsigned integer value
   * This handles regular unsigned int type without ambiguity
   *
   * @param value Value to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(unsigned int value, uint32_t id = 0);

  /**
   * @brief Sends an integer value (8-bit)
   *
   * @param value Value to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(int8_t value, uint32_t id = 0);

  /**
   * @brief Sends an unsigned integer value (8-bit)
   *
   * @param value Value to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(uint8_t value, uint32_t id = 0);
  
  /**
   * @brief Sends an integer value (16-bit)
   *
   * @param value Value to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(int16_t value, uint32_t id = 0);

  /**
   * @brief Sends an unsigned integer value (16-bit)
   *
   * @param value Value to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(uint16_t value, uint32_t id = 0);

#if !defined(__riscv) || !defined(CH32V20x)
  // Only include these if we're not on RISC-V CH32V platform where 
  // long/int32_t and unsigned long/uint32_t are the same types
  
  /**
   * @brief Sends a long integer value
   *
   * @param value Value to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(long value, uint32_t id = 0);

  /**
   * @brief Sends an unsigned long integer value
   *
   * @param value Value to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(unsigned long value, uint32_t id = 0);
#endif

  /**
   * @brief Sends an integer value (32-bit)
   *
   * @param value Value to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(int32_t value, uint32_t id = 0);

  /**
   * @brief Sends an unsigned integer value (32-bit)
   *
   * @param value Value to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(uint32_t value, uint32_t id = 0);

  /**
   * @brief Sends a long integer value (64-bit)
   *
   * @param value Value to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(int64_t value, uint32_t id = 0);

  /**
   * @brief Sends an unsigned long integer value (64-bit)
   *
   * @param value Value to send
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(uint64_t value, uint32_t id = 0);

  /**
   * @brief Sends raw binary data with a specified type
   *
   * @param data Pointer to the data to send
   * @param length Length of the data in bytes
   * @param id Optional CAN ID to use for this message (default: use current txId)
   * @param type Message type to use (default: ZBCAN_TYPE_RAW)
   * @return 0 - Success, 1 - Failure
   */
  uint8_t send(const uint8_t* data, size_t length, uint32_t id = 0, uint8_t type = ZBCAN_TYPE_RAW);

  /*----- CALLBACK REGISTRATION -----*/
  
  /**
   * @brief Register a callback for all message types (no parameters)
   * 
   * @param callback Function to call when any message is received
   */
  void onReceive(ZBCanCallback callback);

  /**
   * @brief Register a callback for string messages (no parameters)
   * 
   * @param callback Function to call when a string message is received
   */
  void onString(ZBCanCallback callback);

  /**
   * @brief Register a callback for integer messages (no parameters)
   * 
   * @param callback Function to call when an integer message is received
   */
  void onInt(ZBCanCallback callback);

  /**
   * @brief Register a callback for unsigned integer messages (no parameters)
   * 
   * @param callback Function to call when an unsigned integer message is received
   */
  void onUint(ZBCanCallback callback);

  /**
   * @brief Register a callback for long integer messages (no parameters)
   * 
   * @param callback Function to call when a long integer message is received
   */
  void onLong(ZBCanCallback callback);

  /**
   * @brief Register a callback for unsigned long integer messages (no parameters)
   * 
   * @param callback Function to call when an unsigned long integer message is received
   */
  void onUlong(ZBCanCallback callback);

  /*----- BACKWARD COMPATIBILITY -----*/
  
  /**
   * @brief Register a callback with parameters (backward compatibility)
   * 
   * @param callback Function to call when any message is received
   */
  __attribute__((deprecated("Use the parameterless version instead")))
  void onReceiveWithParams(ZBCanParamCallback callback);

  /**
   * @brief Register a callback for string messages (backward compatibility)
   * 
   * @param callback Function to call when a string message is received
   */
  __attribute__((deprecated("Use the parameterless version instead")))
  void onStringWithParams(ZBCanStringCallback callback);

  /**
   * @brief Register a callback for integer messages (backward compatibility)
   * 
   * @param callback Function to call when an integer message is received
   */
  __attribute__((deprecated("Use the parameterless version instead")))
  void onIntWithParams(ZBCanIntCallback callback);

  /**
   * @brief Register a callback for unsigned integer messages (backward compatibility)
   * 
   * @param callback Function to call when an unsigned integer message is received
   */
  __attribute__((deprecated("Use the parameterless version instead")))
  void onUintWithParams(ZBCanUintCallback callback);

  /**
   * @brief Register a callback for long integer messages (backward compatibility)
   * 
   * @param callback Function to call when a long integer message is received
   */
  __attribute__((deprecated("Use the parameterless version instead")))
  void onLongWithParams(ZBCanLongCallback callback);

  /**
   * @brief Register a callback for unsigned long integer messages (backward compatibility)
   * 
   * @param callback Function to call when an unsigned long integer message is received
   */
  __attribute__((deprecated("Use the parameterless version instead")))
  void onUlongWithParams(ZBCanUlongCallback callback);

  /**
   * @brief Check if a complete message is available in the queue
   * 
   * @return true if at least one message is available, false otherwise
   */
  bool messageAvailable();

  /**
   * @brief Get the received message and remove it from the queue
   * 
   * @param buffer Buffer to store the message (must be at least ZBCAN_MAX_BUFFER_SIZE+1 bytes)
   * @param type Pointer to store the message type
   * @return Message length or 0 if no message available
   */
  size_t receiveMessage(uint8_t* buffer, uint8_t* type = nullptr);

  /**
   * @brief Alias for send(const char*, uint32_t) - for backward compatibility
   */
  uint8_t sendMessage(const char *message, uint32_t id = 0) { return send(message, id); }
  
  /**
   * @brief Get the CAN ID of the last received message
   * 
   * @return Last received CAN ID
   */
  uint32_t getRxId() { return _rxCanId; }

  /**
   * @brief Get the type of the last received message
   * 
   * @return Last received message type
   */
  uint8_t getLastMessageType() { return _lastMsgType; }

  /**
   * @brief Get message type in user-friendly format
   * 
   * @return User-friendly message type (ZBCAN_MSG_* constants)
   */
  uint8_t getMessageType() {
    if (isString()) return ZBCAN_MSG_STRING;
    if (isInt()) return ZBCAN_MSG_INT;
    if (isUint()) return ZBCAN_MSG_UINT;
    if (isLong()) return ZBCAN_MSG_LONG;
    if (isUlong()) return ZBCAN_MSG_ULONG;
    return ZBCAN_MSG_RAW;
  }

  /*----- CONFIGURATION METHODS -----*/

  /**
   * @brief Sets the baud rate for CAN communication
   * 
   * @param baud Baudrate in bps (e.g., 500000 for 500kbps)
   * @return true if successful, false otherwise
   */
  bool setSpeed(uint32_t baud);

  /**
   * @brief Change CAN mode after initialization
   *
   * @param mode CAN mode (ZBCAN_MODE_NORMAL, ZBCAN_MODE_LOOPBACK, etc.)
   * @return true if mode changed successfully, false otherwise
   */
  bool setMode(uint8_t mode);

  /**
   * @brief Get the current CAN mode
   *
   * @return Current mode (ZBCAN_MODE_NORMAL, ZBCAN_MODE_LOOPBACK, etc.)
   */
  uint8_t getMode(void) { return _mode; }

  /**
   * @brief Get the current CAN Sync Jump Width value
   * @return Value of the SJW parameter (1-4)
   */
  uint8_t getSJW(void);

  /**
   * @brief Get the current CAN Time Segment 1 value
   * @return Value of the BS1 parameter (1-16)
   */
  uint8_t getTimeSeg1(void);

  /**
   * @brief Get the current CAN Time Segment 2 value
   * @return Value of the BS2 parameter (1-8)
   */
  uint8_t getTimeSeg2(void);

  /**
   * @brief Get the current CAN Prescaler value
   * @return Value of the Prescaler parameter (1-1024)
   */
  uint16_t getPrescaler(void);

  /**
   * @brief Calculate the current baudrate based on timing parameters
   * @return Current baudrate in bits per second
   */
  uint32_t getCurrentBaudRate(void);

  /**
   * @brief Set the CAN ID for transmitting messages
   *
   * @param id CAN ID to set
   */
  void setTxId(uint32_t id) { _txCanId = id; }

  /**
   * @brief Set the frame format for CAN messages
   *
   * @param format Frame format (ZBCAN_STANDARD_FRAME or ZBCAN_EXTENDED_FRAME)
   */
  void setFrameFormat(uint8_t format) { _frameFormat = format; }

  /*----- FILTER METHODS -----*/

  /**
   * @brief Set a filter to accept specific CAN ID(s)
   * 
   * @param id CAN ID to accept
   * @param mask Mask to apply (0 bits are don't care)
   * @return true if filter set successfully, false otherwise
   */
  bool setFilter(uint32_t id, uint32_t mask = 0xFFFFFFFF);

  /**
   * @brief Configure filter to accept a range of CAN IDs
   * 
   * @param txId TX ID to set (0 = don't change current TX ID)
   * @param idStart Start of ID range (inclusive)
   * @param idEnd End of ID range (inclusive)
   * @return true if successful, false otherwise
   */
  bool filterByRange(uint32_t txId, uint32_t idStart, uint32_t idEnd);

  /**
   * @brief Configure filter to match the most significant bits of CAN IDs
   * 
   * @param txId TX ID to set (0 = don't change current TX ID)
   * @param msbBits Value of the most significant bits to match
   * @param numBits Number of most significant bits to consider (1-11 for standard frame)
   * @return true if successful, false otherwise
   */
  bool filterByMSB(uint32_t txId, uint32_t msbBits, uint8_t numBits);

  /**
   * @brief Configure multiple CAN ID filters
   * @param ids Array of CAN IDs to accept
   * @param count Number of IDs in the array
   * @return true if filter set successfully, false otherwise
   */
  bool configureMultipleFilters(uint32_t* ids, uint8_t count);

  /**
   * @brief Accept all CAN IDs (no filtering)
   * @return true if filter set successfully, false otherwise
   */
  bool acceptAllIds();

  /*----- HELPER METHODS FOR MESSAGE TYPES -----*/
  
  /**
   * @brief Check if last message is a string
   * @return true if last message is a string, false otherwise
   */
  bool isString() { return _lastMsgType == ZBCAN_TYPE_STRING; }
  
  /**
   * @brief Check if last message is an integer
   * @return true if last message is an integer, false otherwise
   */
  bool isInt() { 
    return _lastMsgType == ZBCAN_TYPE_INT8 || 
           _lastMsgType == ZBCAN_TYPE_INT16 || 
           _lastMsgType == ZBCAN_TYPE_INT32; 
  }
  
  /**
   * @brief Check if last message is an unsigned integer
   * @return true if last message is an unsigned integer, false otherwise
   */
  bool isUint() { 
    return _lastMsgType == ZBCAN_TYPE_UINT8 || 
           _lastMsgType == ZBCAN_TYPE_UINT16 || 
           _lastMsgType == ZBCAN_TYPE_UINT32; 
  }
  
  /**
   * @brief Check if last message is a long integer
   * @return true if last message is a long integer, false otherwise
   */
  bool isLong() { return _lastMsgType == ZBCAN_TYPE_INT64; }
  
  /**
   * @brief Check if last message is an unsigned long integer
   * @return true if last message is an unsigned long integer, false otherwise
   */
  bool isUlong() { return _lastMsgType == ZBCAN_TYPE_UINT64; }
  
  /**
   * @brief Check if last message is raw data
   * @return true if last message is raw data, false otherwise
   */
  bool isRaw() { return _lastMsgType == ZBCAN_TYPE_RAW; }

  /*----- GETTERS FOR LAST RECEIVED VALUES -----*/
  
  /**
   * @brief Get the last received string message
   * @return Pointer to the last string message
   */
  const char* getLastStringMessage() { return _lastStringMessage; }
  
  /**
   * @brief Get the last received 32-bit integer value
   * @return Last received integer value
   */
  int32_t getLastIntValue() { return _lastIntValue; }
  
  /**
   * @brief Get the last received 32-bit unsigned integer value
   * @return Last received unsigned integer value
   */
  uint32_t getLastUintValue() { return _lastUintValue; }
  
  /**
   * @brief Get the last received 64-bit integer value
   * @return Last received long integer value
   */
  int64_t getLastLongValue() { return _lastLongValue; }
  
  /**
   * @brief Get the last received 64-bit unsigned integer value
   * @return Last received unsigned long integer value
   */
  uint64_t getLastUlongValue() { return _lastUlongValue; }
  
  /**
   * @brief Get the last received raw data
   * @return Pointer to the last raw data
   */
  const uint8_t* getLastRawData() { return _lastRawData; }
  
  /**
   * @brief Get the length of the last received raw data
   * @return Length of the last raw data
   */
  size_t getLastRawDataLength() { return _lastRawDataLength; }
  
  /**
   * @brief Get the string length (for string messages)
   * @return Length of the string or raw data length
   */
  size_t getStringLength() { 
    if (isString()) {
      return strlen(_lastStringMessage);
    }
    return _lastRawDataLength;
  }

  /*----- BACKWARD COMPATIBILITY METHODS -----*/
  
  /**
   * @brief For backward compatibility with legacy simple callbacks
   * 
   * @param callback Function to call when a message is received
   */
  __attribute__((deprecated("Use onReceive() instead")))
  void onSimpleReceive(ZBCanCallback callback) { onReceive(callback); }
  
  /**
   * @brief For backward compatibility with legacy simple callbacks
   * 
   * @param callback Function to call when a string message is received
   */
  __attribute__((deprecated("Use onString() instead")))
  void onSimpleString(ZBCanCallback callback) { onString(callback); }
  
  /**
   * @brief For backward compatibility with legacy simple callbacks
   * 
   * @param callback Function to call when an integer message is received
   */
  __attribute__((deprecated("Use onInt() instead")))
  void onSimpleInt(ZBCanCallback callback) { onInt(callback); }
  
  /**
   * @brief For backward compatibility with legacy simple callbacks
   * 
   * @param callback Function to call when an unsigned integer message is received
   */
  __attribute__((deprecated("Use onUint() instead")))
  void onSimpleUint(ZBCanCallback callback) { onUint(callback); }
  
  /**
   * @brief For backward compatibility with legacy simple callbacks
   * 
   * @param callback Function to call when a long integer message is received
   */
  __attribute__((deprecated("Use onLong() instead")))
  void onSimpleLong(ZBCanCallback callback) { onLong(callback); }
  
  /**
   * @brief For backward compatibility with legacy simple callbacks
   * 
   * @param callback Function to call when an unsigned long integer message is received
   */
  __attribute__((deprecated("Use onUlong() instead")))
  void onSimpleUlong(ZBCanCallback callback) { onUlong(callback); }

  /*----- ADVANCED/INTERNAL METHODS -----*/

  /**
   * @brief Process a received CAN frame from the interrupt handler
   *        (For internal use by the interrupt handler)
   * 
   * @param rxMsg Pointer to the received CAN message structure
   */
  void processInterruptFrame(CanRxMsg* rxMsg);

  /**
   * @brief Reset the receiver to prepare for a new message
   */
  void resetReceiver(void);

  /**
   * @brief Reinitialize the CAN peripheral with current settings
   * Used for recovery from error states
   */
  void reinitialize(void);

  /**
   * @brief Check CAN bus status for errors
   * @return true if bus is okay, false if errors detected
   */
  bool checkBusStatus(void);

  /**
   * @brief Recover from CAN bus errors
   */
  void recoverFromErrors(void);
  
private:
  uint8_t _mode;
  bool _isInitialized;
  uint32_t _SyncJumpWidth;
  uint32_t _TimeSeg1;
  uint32_t _TimeSeg2;
  uint32_t _Prescaler;
  
  // Error tracking
  uint16_t _errorCount;
  bool _busOffState;
  
  // Internal buffer for transmission
  uint8_t *_canTxBuffer;
  size_t _canTxBufferSize;
  size_t _canTxBufferAllocated;

  // Message queue for multiple messages
  ZBCanMessage _msgQueue[ZBCAN_MESSAGE_QUEUE_SIZE];
  uint8_t _queueHead;
  uint8_t _queueTail;
  uint8_t _queueCount;
  uint8_t _lastMsgType;

  // Buffer for message reassembly (receiving)
  uint8_t _rxBuffer[ZBCAN_MAX_BUFFER_SIZE];
  uint8_t _rxFramesReceived[ZBCAN_MAX_EXPECTED_FRAMES];
  size_t _rxBufferSize;
  bool _messageComplete;
  uint8_t _highestFrameIndex;
  uint8_t _rxMsgType;

  // Configuration
  uint8_t _frameFormat;
  uint32_t _txCanId;
  uint32_t _rxCanId;

  // Rate limiting
  unsigned long _lastSendTime;
  const unsigned long _minimumSendInterval = 5; // 5ms minimum interval between messages
  
  // Callback related
  ZBCanCallback _callback;
  ZBCanCallback _stringCallback;
  ZBCanCallback _intCallback;
  ZBCanCallback _uintCallback;
  ZBCanCallback _longCallback;
  ZBCanCallback _ulongCallback;
  bool _callbacksEnabled;
  
  // Legacy callbacks with parameters
  ZBCanParamCallback _legacyCallback;
  ZBCanStringCallback _legacyStringCallback;
  ZBCanIntCallback _legacyIntCallback;
  ZBCanUintCallback _legacyUintCallback;
  ZBCanLongCallback _legacyLongCallback;
  ZBCanUlongCallback _legacyUlongCallback;
  
  // Last received message data
  char _lastStringMessage[ZBCAN_MAX_BUFFER_SIZE + 1];
  int32_t _lastIntValue;
  uint32_t _lastUintValue;
  int64_t _lastLongValue;
  uint64_t _lastUlongValue;
  uint8_t _lastRawData[ZBCAN_MAX_BUFFER_SIZE + 1];
  size_t _lastRawDataLength;

  /**
   * @brief Add a complete message to the queue
   * 
   * @param data Message data
   * @param length Message length
   * @param id CAN ID of the message
   * @param type Message type
   * @return true if message was added successfully, false if queue is full
   */
  bool enqueueMessage(uint8_t* data, size_t length, uint32_t id, uint8_t type);

  /**
   * @brief Process any pending messages in the message queue via callback
   *        (Internal use only - called automatically when needed)
   */
  void processCallbacks();

  /**
   * @brief Allocates memory for the CAN transmission buffer
   *
   * @param length Required buffer size
   * @return Allocated buffer size or 0 if allocation failed
   */
  size_t _allocateCanTxBuffer(size_t length);

  /**
   * @brief Sends a single CAN frame
   *
   * @param data Data to send
   * @param len Data length (max 6 bytes due to metadata)
   * @param frameId CAN ID to use
   * @param frameIndex Frame sequence number
   * @param isLastFrame Flag indicating last frame in sequence
   * @param type Message type
   * @return 0 - Success, 1 - Failure
   */
  uint8_t _sendFrame(uint8_t *data, uint8_t len, uint32_t frameId, uint8_t frameIndex, bool isLastFrame, uint8_t type);

  /**
   * @brief Processes a received CAN frame and adds it to the message buffer
   *
   * @param data Frame data including metadata
   * @param length Data length
   * @param canId CAN ID of the received frame
   * @return 1 if message is complete, 0 if more frames expected
   */
  uint8_t _processFrame(uint8_t *data, uint8_t length, uint32_t canId);
  
  /**
   * @brief Buffers data for CAN transmission
   *
   * @param data Data to buffer
   * @param size Size of data
   * @return Amount of data buffered or 0 if failed
   */
  size_t write(const uint8_t *data, size_t size);

  /**
   * @brief Sends all buffered data as CAN frames
   *
   * @param id CAN ID to use for transmission (0 = use current txId)
   * @param type Message type
   * @return 0 - All frames sent successfully, 1 - Error occurred
   */
  uint8_t endTransmission(uint32_t id = 0, uint8_t type = ZBCAN_TYPE_STRING);

  /**
   * @brief Set CAN baudrate parameters directly
   *
   * @param prescaler Clock prescaler (1-1024)
   * @param timeseg1 Time segment 1 (1-16)
   * @param timeseg2 Time segment 2 (1-8)
   * @param sjw Synchronization jump width (1-4)
   */
  void setBaudRateValues(uint16_t prescaler, uint8_t timeseg1, uint8_t timeseg2, uint8_t sjw);

  /**
   * @brief Initialize CAN with manual bit timing parameters
   *
   * @param tsjw CAN synchronisation jump width
   * @param tbs2 CAN time quantum in bit segment 2
   * @param tbs1 CAN time quantum in bit segment 1
   * @param brp Specifies the length of a time quantum
   * @param mode Test mode
   * @param txId CAN ID to use for transmitting
   * @param frameFormat Frame format (ZBCAN_STANDARD_FRAME or ZBCAN_EXTENDED_FRAME)
   * @return true if initialization successful, false otherwise
   */
  bool beginManual(uint8_t tsjw, uint8_t tbs2, uint8_t tbs1, uint16_t brp, 
                  uint8_t mode = ZBCAN_MODE_NORMAL, uint32_t txId = 0x100, 
                  uint8_t frameFormat = ZBCAN_STANDARD_FRAME);

  /**
   * @brief Configure filter with mask for CAN ID filtering
   *
   * @param txId TX ID to set (0 = don't change current TX ID)
   * @param idMask Mask that specifies which bits to consider (1 = consider, 0 = ignore)
   * @param idPattern Pattern to match against the considered bits
   * @return true if successful, false otherwise
   */
  bool configureFilterMask(uint32_t txId, uint32_t idMask, uint32_t idPattern);
};

extern ZBCan ZBCan1;

// Interrupt handler declaration - We use only FIFO1
extern "C" {
  void CAN1_RX1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
}
#endif
// ZBCAN.cpp - Implementation of user-friendly CAN interface with parameterless callbacks
#include "ZBCAN.h"

ZBCan ZBCan1;

// Global variable to store pointer to the active ZBCan instance for interrupt handling
static ZBCan* activeCANInstance = nullptr;

ZBCan::ZBCan()
{
  _canTxBuffer = nullptr;
  _canTxBufferSize = 0;
  _canTxBufferAllocated = 0;
  _frameFormat = ZBCAN_STANDARD_FRAME;
  _txCanId = 0x100;
  _rxCanId = 0x0;
  _messageComplete = false;
  _isInitialized = false;
  _lastMsgType = ZBCAN_TYPE_STRING;
  _rxMsgType = ZBCAN_TYPE_STRING;
  
  // Error tracking
  _errorCount = 0;
  _busOffState = false;

  // Initialize timing parameters to zero to indicate they haven't been set
  _SyncJumpWidth = 0;
  _TimeSeg1 = 0;
  _TimeSeg2 = 0;
  _Prescaler = 0;

  // Initialize message queue
  _queueHead = 0;
  _queueTail = 0;
  _queueCount = 0;
  for (uint8_t i = 0; i < ZBCAN_MESSAGE_QUEUE_SIZE; i++) {
    _msgQueue[i].valid = false;
    _msgQueue[i].length = 0;
    _msgQueue[i].type = ZBCAN_TYPE_STRING;
  }

  // Initialize callback related members
  _callback = nullptr;
  _stringCallback = nullptr;
  _intCallback = nullptr;
  _uintCallback = nullptr;
  _longCallback = nullptr;
  _ulongCallback = nullptr;
  _callbacksEnabled = false;
  
  // Initialize legacy callbacks
  _legacyCallback = nullptr;
  _legacyStringCallback = nullptr;
  _legacyIntCallback = nullptr;
  _legacyUintCallback = nullptr;
  _legacyLongCallback = nullptr;
  _legacyUlongCallback = nullptr;
  
  // Initialize message storage
  memset(_lastStringMessage, 0, ZBCAN_MAX_BUFFER_SIZE + 1);
  _lastIntValue = 0;
  _lastUintValue = 0;
  _lastLongValue = 0;
  _lastUlongValue = 0;
  memset(_lastRawData, 0, ZBCAN_MAX_BUFFER_SIZE + 1);
  _lastRawDataLength = 0;

  // Initialize rate limiting
  _lastSendTime = 0;

  resetReceiver();
}

// Callback registration methods
void ZBCan::onReceive(ZBCanCallback callback) {
  _callback = callback;
  
  // Automatically enable callbacks when a callback is registered
  if (callback != nullptr) {
    _callbacksEnabled = true;
  } else if (_stringCallback == nullptr && _intCallback == nullptr && 
             _uintCallback == nullptr && _longCallback == nullptr && 
             _ulongCallback == nullptr && _legacyCallback == nullptr &&
             _legacyStringCallback == nullptr && _legacyIntCallback == nullptr &&
             _legacyUintCallback == nullptr && _legacyLongCallback == nullptr &&
             _legacyUlongCallback == nullptr) {
    _callbacksEnabled = false; // Disable if no callbacks are registered
  }
}

void ZBCan::onString(ZBCanCallback callback) {
  _stringCallback = callback;
  
  // Automatically enable callbacks when a callback is registered
  if (callback != nullptr) {
    _callbacksEnabled = true;
  } else if (_callback == nullptr && _intCallback == nullptr && 
             _uintCallback == nullptr && _longCallback == nullptr && 
             _ulongCallback == nullptr && _legacyCallback == nullptr &&
             _legacyStringCallback == nullptr && _legacyIntCallback == nullptr &&
             _legacyUintCallback == nullptr && _legacyLongCallback == nullptr &&
             _legacyUlongCallback == nullptr) {
    _callbacksEnabled = false; // Disable if no callbacks are registered
  }
}

void ZBCan::onInt(ZBCanCallback callback) {
  _intCallback = callback;
  
  // Automatically enable callbacks when a callback is registered
  if (callback != nullptr) {
    _callbacksEnabled = true;
  } else if (_callback == nullptr && _stringCallback == nullptr && 
             _uintCallback == nullptr && _longCallback == nullptr && 
             _ulongCallback == nullptr && _legacyCallback == nullptr &&
             _legacyStringCallback == nullptr && _legacyIntCallback == nullptr &&
             _legacyUintCallback == nullptr && _legacyLongCallback == nullptr &&
             _legacyUlongCallback == nullptr) {
    _callbacksEnabled = false; // Disable if no callbacks are registered
  }
}

void ZBCan::onUint(ZBCanCallback callback) {
  _uintCallback = callback;
  
  // Automatically enable callbacks when a callback is registered
  if (callback != nullptr) {
    _callbacksEnabled = true;
  } else if (_callback == nullptr && _stringCallback == nullptr && 
             _intCallback == nullptr && _longCallback == nullptr && 
             _ulongCallback == nullptr && _legacyCallback == nullptr &&
             _legacyStringCallback == nullptr && _legacyIntCallback == nullptr &&
             _legacyUintCallback == nullptr && _legacyLongCallback == nullptr &&
             _legacyUlongCallback == nullptr) {
    _callbacksEnabled = false; // Disable if no callbacks are registered
  }
}

void ZBCan::onLong(ZBCanCallback callback) {
  _longCallback = callback;
  
  // Automatically enable callbacks when a callback is registered
  if (callback != nullptr) {
    _callbacksEnabled = true;
  } else if (_callback == nullptr && _stringCallback == nullptr && 
             _intCallback == nullptr && _uintCallback == nullptr && 
             _ulongCallback == nullptr && _legacyCallback == nullptr &&
             _legacyStringCallback == nullptr && _legacyIntCallback == nullptr &&
             _legacyUintCallback == nullptr && _legacyLongCallback == nullptr &&
             _legacyUlongCallback == nullptr) {
    _callbacksEnabled = false; // Disable if no callbacks are registered
  }
}

void ZBCan::onUlong(ZBCanCallback callback) {
  _ulongCallback = callback;
  
  // Automatically enable callbacks when a callback is registered
  if (callback != nullptr) {
    _callbacksEnabled = true;
  } else if (_callback == nullptr && _stringCallback == nullptr && 
             _intCallback == nullptr && _uintCallback == nullptr && 
             _longCallback == nullptr && _legacyCallback == nullptr &&
             _legacyStringCallback == nullptr && _legacyIntCallback == nullptr &&
             _legacyUintCallback == nullptr && _legacyLongCallback == nullptr &&
             _legacyUlongCallback == nullptr) {
    _callbacksEnabled = false; // Disable if no callbacks are registered
  }
}

// Legacy callback registration methods
void ZBCan::onReceiveWithParams(ZBCanParamCallback callback) {
  _legacyCallback = callback;
  
  // Automatically enable callbacks when a callback is registered
  if (callback != nullptr) {
    _callbacksEnabled = true;
  } else if (_callback == nullptr && _stringCallback == nullptr && 
             _intCallback == nullptr && _uintCallback == nullptr && 
             _longCallback == nullptr && _ulongCallback == nullptr &&
             _legacyStringCallback == nullptr && _legacyIntCallback == nullptr &&
             _legacyUintCallback == nullptr && _legacyLongCallback == nullptr &&
             _legacyUlongCallback == nullptr) {
    _callbacksEnabled = false; // Disable if no callbacks are registered
  }
}

void ZBCan::onStringWithParams(ZBCanStringCallback callback) {
  _legacyStringCallback = callback;
  
  // Automatically enable callbacks when a callback is registered
  if (callback != nullptr) {
    _callbacksEnabled = true;
  } else if (_callback == nullptr && _stringCallback == nullptr && 
             _intCallback == nullptr && _uintCallback == nullptr && 
             _longCallback == nullptr && _ulongCallback == nullptr &&
             _legacyCallback == nullptr && _legacyIntCallback == nullptr &&
             _legacyUintCallback == nullptr && _legacyLongCallback == nullptr &&
             _legacyUlongCallback == nullptr) {
    _callbacksEnabled = false; // Disable if no callbacks are registered
  }
}

void ZBCan::onIntWithParams(ZBCanIntCallback callback) {
  _legacyIntCallback = callback;
  
  // Similar pattern for other callback registrations
  if (callback != nullptr) {
    _callbacksEnabled = true;
  } else if (_callback == nullptr && _stringCallback == nullptr && 
             _intCallback == nullptr && _uintCallback == nullptr && 
             _longCallback == nullptr && _ulongCallback == nullptr &&
             _legacyCallback == nullptr && _legacyStringCallback == nullptr &&
             _legacyUintCallback == nullptr && _legacyLongCallback == nullptr &&
             _legacyUlongCallback == nullptr) {
    _callbacksEnabled = false;
  }
}

void ZBCan::onUintWithParams(ZBCanUintCallback callback) {
  _legacyUintCallback = callback;
  
  if (callback != nullptr) {
    _callbacksEnabled = true;
  } else if (_callback == nullptr && _stringCallback == nullptr && 
             _intCallback == nullptr && _uintCallback == nullptr && 
             _longCallback == nullptr && _ulongCallback == nullptr &&
             _legacyCallback == nullptr && _legacyStringCallback == nullptr &&
             _legacyIntCallback == nullptr && _legacyLongCallback == nullptr &&
             _legacyUlongCallback == nullptr) {
    _callbacksEnabled = false;
  }
}

void ZBCan::onLongWithParams(ZBCanLongCallback callback) {
  _legacyLongCallback = callback;
  
  if (callback != nullptr) {
    _callbacksEnabled = true;
  } else if (_callback == nullptr && _stringCallback == nullptr && 
             _intCallback == nullptr && _uintCallback == nullptr && 
             _longCallback == nullptr && _ulongCallback == nullptr &&
             _legacyCallback == nullptr && _legacyStringCallback == nullptr &&
             _legacyIntCallback == nullptr && _legacyUintCallback == nullptr &&
             _legacyUlongCallback == nullptr) {
    _callbacksEnabled = false;
  }
}

void ZBCan::onUlongWithParams(ZBCanUlongCallback callback) {
  _legacyUlongCallback = callback;
  
  if (callback != nullptr) {
    _callbacksEnabled = true;
  } else if (_callback == nullptr && _stringCallback == nullptr && 
             _intCallback == nullptr && _uintCallback == nullptr && 
             _longCallback == nullptr && _ulongCallback == nullptr &&
             _legacyCallback == nullptr && _legacyStringCallback == nullptr &&
             _legacyIntCallback == nullptr && _legacyUintCallback == nullptr &&
             _legacyLongCallback == nullptr) {
    _callbacksEnabled = false;
  }
}

bool ZBCan::begin() 
{
  // Default to 500kbps in normal mode with standard frames
  return begin(500000, ZBCAN_MODE_NORMAL, 0x100, ZBCAN_STANDARD_FRAME);
}

bool ZBCan::begin(uint32_t baud, uint8_t mode, uint32_t txId, uint8_t frameFormat)
{
  // Store values
  _mode = mode;
  _txCanId = txId;
  _frameFormat = frameFormat;
  
  // Calculate bit timing parameters
  if (!setSpeed(baud)) {
    return false; // Failed to calculate bit timing for requested baudrate
  }
  
  // Initialize with calculated parameters
  return beginManual(_SyncJumpWidth, _TimeSeg2, _TimeSeg1, _Prescaler, mode, txId, frameFormat);
}

bool ZBCan::beginManual(uint8_t tsjw, uint8_t tbs2, uint8_t tbs1, uint16_t brp, 
                       uint8_t mode, uint32_t txId, uint8_t frameFormat)
{
  GPIO_InitTypeDef GPIO_InitStructure = {0};
  CAN_InitTypeDef CAN_InitStructure = {0};
  CAN_FilterInitTypeDef CAN_FilterInitStructure = {0};
  NVIC_InitTypeDef NVIC_InitStructure = {0};

  // Store configuration
  _mode = mode;
  _frameFormat = frameFormat;
  _txCanId = txId;
  
  // Store bit timing values
  _SyncJumpWidth = tsjw;
  _TimeSeg2 = tbs2;
  _TimeSeg1 = tbs1;
  _Prescaler = brp;

  // Enable peripheral clocks
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

  // Remap CAN pins
  GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

  // Configure CAN_TX pin (PB9)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // Configure CAN_RX pin (PB8)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // Initialize CAN controller
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = ENABLE;  // Enable automatic bus-off recovery
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = ENABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = _mode;
  CAN_InitStructure.CAN_SJW = tsjw;
  CAN_InitStructure.CAN_BS1 = tbs1;
  CAN_InitStructure.CAN_BS2 = tbs2;
  CAN_InitStructure.CAN_Prescaler = brp;
  uint8_t initStatus = CAN_Init(CAN1, &CAN_InitStructure);
  
  if (initStatus != CAN_InitStatus_Success) {
    return false;
  }

  // Initialize CAN filter to pass any message (default)
  acceptAllIds();

  // Set up NVIC for CAN1 RX1 interrupt
  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Enable FIFO1 message pending Interrupt and error interrupts
  CAN_ITConfig(CAN1, CAN_IT_FMP1 | CAN_IT_BOF | CAN_IT_EPV | CAN_IT_EWG, ENABLE);

  // Reset error flags
  _errorCount = 0;
  _busOffState = false;

  // Store global instance for interrupt handling
  activeCANInstance = this;

  // Set initialization flag
  _isInitialized = true;
  
  return true;
}

bool ZBCan::setMode(uint8_t mode)
{
  // Store the requested mode
  _mode = mode;

  // If hardware is not initialized yet, just store the mode for later use in begin()
  if (!_isInitialized) {
    return true; // Success - mode will be applied during begin()
  }

  // If hardware is initialized, reconfigure it with the new mode
  CAN_InitTypeDef CAN_InitStructure = {0};

  // Apply current configuration
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = ENABLE;  // Enable automatic bus-off recovery
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = ENABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = mode;  // Apply the new mode
  CAN_InitStructure.CAN_SJW = _SyncJumpWidth;
  CAN_InitStructure.CAN_BS1 = _TimeSeg1;
  CAN_InitStructure.CAN_BS2 = _TimeSeg2;
  CAN_InitStructure.CAN_Prescaler = _Prescaler;

  // Apply the new configuration to the CAN peripheral
  uint8_t status = CAN_Init(CAN1, &CAN_InitStructure);

  // Reset receiver state to ensure clean start
  resetReceiver();

  return status == CAN_InitStatus_Success;
}

void ZBCan::processCallbacks() 
{
  // If callbacks aren't enabled or no callbacks are registered, return early
  if (!_callbacksEnabled || 
      (_callback == nullptr && _stringCallback == nullptr && 
       _intCallback == nullptr && _uintCallback == nullptr && 
       _longCallback == nullptr && _ulongCallback == nullptr &&
       _legacyCallback == nullptr && _legacyStringCallback == nullptr &&
       _legacyIntCallback == nullptr && _legacyUintCallback == nullptr &&
       _legacyLongCallback == nullptr && _legacyUlongCallback == nullptr)) {
    return;
  }

  // Process all queued messages via appropriate callbacks
  while (messageAvailable()) {
    // Local buffer to hold the message
    uint8_t buffer[ZBCAN_MAX_BUFFER_SIZE + 1];
    uint8_t msgType;
    
    // Get the next message from the queue
    size_t msgSize = receiveMessage(buffer, &msgType);
    
    if (msgSize > 0) {
      // Update global message storage based on type
      switch (msgType) {
        case ZBCAN_TYPE_STRING:
          // Ensure string is null-terminated
          buffer[msgSize] = 0;
          strncpy(_lastStringMessage, (const char*)buffer, ZBCAN_MAX_BUFFER_SIZE);
          _lastStringMessage[ZBCAN_MAX_BUFFER_SIZE] = 0; // Ensure null termination
          _lastRawDataLength = msgSize; // Make sure we store the length correctly
          break;
          
        case ZBCAN_TYPE_INT8:
          if (msgSize == 1) {
            _lastIntValue = (int32_t)(*(int8_t*)buffer);
          }
          _lastRawDataLength = msgSize;
          break;
          
        case ZBCAN_TYPE_INT16:
          if (msgSize == 2) {
            _lastIntValue = (int32_t)(*(int16_t*)buffer);
          }
          _lastRawDataLength = msgSize;
          break;
          
        case ZBCAN_TYPE_INT32:
          if (msgSize == 4) {
            _lastIntValue = *(int32_t*)buffer;
          }
          _lastRawDataLength = msgSize;
          break;
          
        case ZBCAN_TYPE_UINT8:
          if (msgSize == 1) {
            _lastUintValue = (uint32_t)(*(uint8_t*)buffer);
          }
          _lastRawDataLength = msgSize;
          break;
          
        case ZBCAN_TYPE_UINT16:
          if (msgSize == 2) {
            _lastUintValue = (uint32_t)(*(uint16_t*)buffer);
          }
          _lastRawDataLength = msgSize;
          break;
          
        case ZBCAN_TYPE_UINT32:
          if (msgSize == 4) {
            _lastUintValue = *(uint32_t*)buffer;
          }
          _lastRawDataLength = msgSize;
          break;
          
        case ZBCAN_TYPE_INT64:
          if (msgSize == 8) {
            _lastLongValue = *(int64_t*)buffer;
          }
          _lastRawDataLength = msgSize;
          break;
          
        case ZBCAN_TYPE_UINT64:
          if (msgSize == 8) {
            _lastUlongValue = *(uint64_t*)buffer;
          }
          _lastRawDataLength = msgSize;
          break;
          
        default:
        case ZBCAN_TYPE_RAW:
          // Copy raw data
          memcpy(_lastRawData, buffer, msgSize);
          _lastRawDataLength = msgSize;
          break;
      }

      // Call the legacy callback with parameters if registered
      if (_legacyCallback != nullptr) {
        _legacyCallback(_rxCanId, buffer, msgSize, msgType);
      }
      
      // Call type-specific legacy callbacks if registered
      switch (msgType) {
        case ZBCAN_TYPE_STRING:
          if (_legacyStringCallback != nullptr) {
            _legacyStringCallback(_rxCanId, (const char*)buffer, msgSize);
          }
          if (_stringCallback != nullptr) {
            _stringCallback();
          }
          break;
          
        case ZBCAN_TYPE_INT8:
        case ZBCAN_TYPE_INT16:
        case ZBCAN_TYPE_INT32:
          if (_legacyIntCallback != nullptr && msgSize <= sizeof(int32_t)) {
            int32_t value = 0;
            // Copy data with appropriate sign extension
            if (msgSize == 1) {
              value = (int32_t)(*(int8_t*)buffer);
            } else if (msgSize == 2) {
              value = (int32_t)(*(int16_t*)buffer);
            } else {
              value = *(int32_t*)buffer;
            }
            _legacyIntCallback(_rxCanId, value);
          }
          if (_intCallback != nullptr) {
            _intCallback();
          }
          break;
          
        case ZBCAN_TYPE_UINT8:
        case ZBCAN_TYPE_UINT16:
        case ZBCAN_TYPE_UINT32:
          if (_legacyUintCallback != nullptr && msgSize <= sizeof(uint32_t)) {
            uint32_t value = 0;
            // Copy data
            if (msgSize == 1) {
              value = (uint32_t)(*(uint8_t*)buffer);
            } else if (msgSize == 2) {
              value = (uint32_t)(*(uint16_t*)buffer);
            } else {
              value = *(uint32_t*)buffer;
            }
            _legacyUintCallback(_rxCanId, value);
          }
          if (_uintCallback != nullptr) {
            _uintCallback();
          }
          break;
          
        case ZBCAN_TYPE_INT64:
          if (_legacyLongCallback != nullptr && msgSize == sizeof(int64_t)) {
            _legacyLongCallback(_rxCanId, *(int64_t*)buffer);
          }
          if (_longCallback != nullptr) {
            _longCallback();
          }
          break;
          
        case ZBCAN_TYPE_UINT64:
          if (_legacyUlongCallback != nullptr && msgSize == sizeof(uint64_t)) {
            _legacyUlongCallback(_rxCanId, *(uint64_t*)buffer);
          }
          if (_ulongCallback != nullptr) {
            _ulongCallback();
          }
          break;
      }
      
      // Call the simple parameterless callback if registered
      if (_callback != nullptr) {
        _callback();
      }
    }
  }
}

// Unified send methods for different data types
// Implementation of the platform-compatible send methods

// Regular int type
uint8_t ZBCan::send(int value, uint32_t id) {
  return send((const uint8_t*)&value, sizeof(value), id, ZBCAN_TYPE_INT32);
}

// Unsigned int type
uint8_t ZBCan::send(unsigned int value, uint32_t id) {
  return send((const uint8_t*)&value, sizeof(value), id, ZBCAN_TYPE_UINT32);
}

// Fixed-size integer implementations

uint8_t ZBCan::send(int8_t value, uint32_t id) {
  return send((const uint8_t*)&value, sizeof(value), id, ZBCAN_TYPE_INT8);
}

uint8_t ZBCan::send(uint8_t value, uint32_t id) {
  return send((const uint8_t*)&value, sizeof(value), id, ZBCAN_TYPE_UINT8);
}

uint8_t ZBCan::send(int16_t value, uint32_t id) {
  return send((const uint8_t*)&value, sizeof(value), id, ZBCAN_TYPE_INT16);
}

uint8_t ZBCan::send(uint16_t value, uint32_t id) {
  return send((const uint8_t*)&value, sizeof(value), id, ZBCAN_TYPE_UINT16);
}

uint8_t ZBCan::send(int32_t value, uint32_t id) {
  return send((const uint8_t*)&value, sizeof(value), id, ZBCAN_TYPE_INT32);
}

uint8_t ZBCan::send(uint32_t value, uint32_t id) {
  return send((const uint8_t*)&value, sizeof(value), id, ZBCAN_TYPE_UINT32);
}

// Platform-specific long and unsigned long implementation
// Only compile these if not on RISC-V CH32V platform
#if !defined(__riscv) || !defined(CH32V20x)
uint8_t ZBCan::send(long value, uint32_t id) {
  return send((const uint8_t*)&value, sizeof(value), id, ZBCAN_TYPE_INT32);
}

uint8_t ZBCan::send(unsigned long value, uint32_t id) {
  return send((const uint8_t*)&value, sizeof(value), id, ZBCAN_TYPE_UINT32);
}
#endif

uint8_t ZBCan::send(int64_t value, uint32_t id) {
  return send((const uint8_t*)&value, sizeof(value), id, ZBCAN_TYPE_INT64);
}

uint8_t ZBCan::send(uint64_t value, uint32_t id) {
  return send((const uint8_t*)&value, sizeof(value), id, ZBCAN_TYPE_UINT64);
}

// Implementation of string send
uint8_t ZBCan::send(const char *message, uint32_t id) {
  if (message == nullptr) {
    return 1; // Invalid parameter
  }
  
  return send((const uint8_t*)message, strlen(message), id, ZBCAN_TYPE_STRING);
}

// General-purpose send with type parameter
uint8_t ZBCan::send(const uint8_t* data, size_t length, uint32_t id, uint8_t type) {
  static uint8_t consecutiveFails = 0;
  
  if (data == nullptr || length == 0 || length > ZBCAN_MAX_BUFFER_SIZE) {
    return 1; // Invalid parameters
  }
  
  // Rate limiting - ensure minimum delay between sends
  unsigned long currentTime = millis();
  if (currentTime - _lastSendTime < _minimumSendInterval) {
    // Not enough time has passed since last send, add a small delay
    delay(_minimumSendInterval - (currentTime - _lastSendTime));
  }
  
  // Check for bus errors before sending
  if (!checkBusStatus()) {
    // Bus error detected, try to recover
    recoverFromErrors();
    
    // Increment fail counter
    consecutiveFails++;
    if (consecutiveFails > 10) {
      consecutiveFails = 10; // Cap at 10
    }
    
    // If we've failed too many times, return failure
    if (consecutiveFails > 5) {
      return 1;
    }
  }
  
  // Add some variability to transmission timing to avoid persistent collisions
  // Only if we've had failures
  if (consecutiveFails > 0) {
    // Small variable delay based on current error count
    for (volatile uint16_t j = 0; j < (consecutiveFails * 100); j++) {
      asm("nop");
    }
  }

  // Write data to buffer
  if (write(data, length) == 0) {
    consecutiveFails++;
    return 1; // Failed to buffer data
  }

  // Send all buffered data
  uint8_t result = endTransmission(id, type);
  
  // Update last send time
  _lastSendTime = millis();
  
  if (result == 0) {
    // Successful transmission
    consecutiveFails = 0;
  } else {
    // Failed transmission
    consecutiveFails++;
  }
  
  return result;
}

bool ZBCan::setFilter(uint32_t id, uint32_t mask)
{
  // Simply pass through to the internal implementation
  return configureFilterMask(0, mask, id);
}

bool ZBCan::acceptAllIds()
{
  CAN_FilterInitTypeDef CAN_FilterInitStructure = {0};

  // Initialize CAN filter to pass any message
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1; // Always use FIFO1

  // Set mask and ID to zero to accept any ID
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  
  CAN_FilterInit(&CAN_FilterInitStructure);
  
  return true;
}

bool ZBCan::configureFilterMask(uint32_t txId, uint32_t idMask, uint32_t idPattern)
{
  CAN_FilterInitTypeDef CAN_FilterInitStructure = {0};

  // Update TX ID if provided (non-zero)
  if (txId != 0x0)
  {
    _txCanId = txId;
  }

  // Initialize CAN filter
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1; // Always use FIFO1
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;

  // Configure mask and pattern based on frame format
  if (_frameFormat == ZBCAN_STANDARD_FRAME)
  {
    // For standard IDs, the ID is shifted 5 bits to the left in the high register
    CAN_FilterInitStructure.CAN_FilterIdHigh = (idPattern << 5) & 0xFFFF;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (idMask << 5) & 0xFFFF;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
  }
  else
  {
    // For extended IDs, the ID is split between high and low registers
    CAN_FilterInitStructure.CAN_FilterIdHigh = (idPattern >> 13) & 0xFFFF;
    CAN_FilterInitStructure.CAN_FilterIdLow = ((idPattern & 0x1FFF) << 3) | 0x04; // Set IDE bit
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (idMask >> 13) & 0xFFFF;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = ((idMask & 0x1FFF) << 3) | 0x04; // Mask IDE bit too
  }

  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  return true;
}

bool ZBCan::configureMultipleFilters(uint32_t* ids, uint8_t count) 
{
  // Currently, STM32 CAN peripherals have up to 14 filter banks
  // We'll use filter banks 0-13 based on how many IDs we need to filter
  
  // Cap the count to a reasonable number
  if (count > 14) count = 14;
  
  // First, disable all filters
  for (uint8_t i = 0; i < 14; i++) {
    CAN_FilterInitTypeDef CAN_FilterInitStructure = {0};
    CAN_FilterInitStructure.CAN_FilterNumber = i;
    CAN_FilterInitStructure.CAN_FilterActivation = DISABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
  }
  
  // Now configure each filter bank for a specific ID
  for (uint8_t i = 0; i < count; i++) {
    CAN_FilterInitTypeDef CAN_FilterInitStructure = {0};
    CAN_FilterInitStructure.CAN_FilterNumber = i;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList; // Use ID list mode
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1;
    
    // Configure filter based on frame format
    if (_frameFormat == ZBCAN_STANDARD_FRAME) {
      // Standard ID
      CAN_FilterInitStructure.CAN_FilterIdHigh = (ids[i] << 5) & 0xFFFF;
      CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
      CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000; // Not used in list mode
      CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;  // Not used in list mode
    } else {
      // Extended ID
      CAN_FilterInitStructure.CAN_FilterIdHigh = (ids[i] >> 13) & 0xFFFF;
      CAN_FilterInitStructure.CAN_FilterIdLow = ((ids[i] & 0x1FFF) << 3) | 0x04; // Set IDE bit
      CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000; // Not used in list mode
      CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;  // Not used in list mode
    }
    
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
  }
  
  return true;
}

bool ZBCan::filterByRange(uint32_t txId, uint32_t idStart, uint32_t idEnd)
{
  // Update TX ID if provided (non-zero)
  if (txId != 0x0)
  {
    _txCanId = txId;
  }

  // This is more complex as we need to find a set of mask/filter pairs that cover the range
  // For simplicity, we'll use a single filter with a mask that covers the common prefix

  // Check if the range is valid
  if (idStart > idEnd)
  {
    return false; // Invalid range
  }

  // Find the common prefix
  uint32_t xorResult = idStart ^ idEnd;
  uint32_t mask = 0;

  // Determine the position of the highest bit that differs
  int diffBit = -1;
  for (int i = (_frameFormat == ZBCAN_STANDARD_FRAME ? 11 : 29) - 1; i >= 0; i--)
  {
    if (xorResult & (1 << i))
    {
      diffBit = i;
      break;
    }
  }

  // Create a mask that includes all bits higher than the highest differing bit
  for (int i = diffBit + 1; i < (_frameFormat == ZBCAN_STANDARD_FRAME ? 11 : 29); i++)
  {
    mask |= (1 << i);
  }

  // Extract the common pattern using the mask
  uint32_t pattern = idStart & mask;

  // Configure the filter with our mask and pattern
  return configureFilterMask(0, mask, pattern);
}

bool ZBCan::filterByMSB(uint32_t txId, uint32_t msbBits, uint8_t numBits)
{
  // Update TX ID if provided (non-zero)
  if (txId != 0x0)
  {
    _txCanId = txId;
  }

  // Limit number of bits based on frame format
  uint8_t maxBits = (_frameFormat == ZBCAN_STANDARD_FRAME) ? 11 : 29;
  if (numBits > maxBits)
  {
    numBits = maxBits;
  }

  // Calculate mask for the MSB bits
  uint32_t mask = 0;
  for (int i = 0; i < numBits; i++)
  {
    mask |= (1 << (maxBits - 1 - i));
  }

  // Position the msbBits pattern correctly
  uint32_t pattern = (msbBits << (maxBits - numBits)) & mask;

  // Configure the filter with our mask and pattern
  return configureFilterMask(0, mask, pattern);
}

bool ZBCan::setSpeed(uint32_t baud)
{
  RCC_ClocksTypeDef RCC_ClocksStatus;
  RCC_GetClocksFreq(&RCC_ClocksStatus);

  // Get the APB1 bus frequency since CAN is on APB1
  uint32_t frequency = RCC_ClocksStatus.PCLK1_Frequency;

  uint16_t prescaler;
  uint8_t bs1, bs2;

  for (prescaler = 1; prescaler <= 1024; prescaler += 1)
  {
    const uint32_t can_freq = frequency / prescaler;
    const uint32_t baud_min = can_freq / (1 + 5 + 16);

    /* skip all prescaler values that can't possibly achieve the desired baudrate */
    if (baud_min > baud)
      continue;

    for (bs2 = 1; bs2 <= 5; bs2 += 1)
    {
      for (bs1 = (bs2 * 3) - 1; bs1 <= 16; bs1 += 1)
      {
        const uint32_t baud_cur = can_freq / (1 + bs1 + bs2);
        if (baud_cur != baud)
          continue;

        setBaudRateValues(prescaler, bs1, bs2, 4);
        return true;
      }
    }
  }

  /* Failed to calculate an acceptable baud rate */
  return false;
}

void ZBCan::setBaudRateValues(uint16_t prescaler, uint8_t timeseg1, uint8_t timeseg2, uint8_t sjw)
{
  /* the CAN specification (v2.0) states that SJW shall be programmable between
   * 1 and min(4, timeseg1)... the bxCAN documentation doesn't mention this
   */
  if (sjw > 4)
    sjw = 4;
  if (sjw > timeseg1)
    sjw = timeseg1;

  switch (sjw)
  {
  case 0:
  case 1:
    _SyncJumpWidth = CAN_SJW_1tq;
    break;
  case 2:
    _SyncJumpWidth = CAN_SJW_2tq;
    break;
  case 3:
    _SyncJumpWidth = CAN_SJW_3tq;
    break;
  case 4:
  default: /* limit to 4 */
    _SyncJumpWidth = CAN_SJW_4tq;
    break;
  }

  switch (timeseg1)
  {
  case 1:
    _TimeSeg1 = CAN_BS1_1tq;
    break;
  case 2:
    _TimeSeg1 = CAN_BS1_2tq;
    break;
  case 3:
    _TimeSeg1 = CAN_BS1_3tq;
    break;
  case 4:
    _TimeSeg1 = CAN_BS1_4tq;
    break;
  case 5:
    _TimeSeg1 = CAN_BS1_5tq;
    break;
  case 6:
    _TimeSeg1 = CAN_BS1_6tq;
    break;
  case 7:
    _TimeSeg1 = CAN_BS1_7tq;
    break;
  case 8:
    _TimeSeg1 = CAN_BS1_8tq;
    break;
  case 9:
    _TimeSeg1 = CAN_BS1_9tq;
    break;
  case 10:
    _TimeSeg1 = CAN_BS1_10tq;
    break;
  case 11:
    _TimeSeg1 = CAN_BS1_11tq;
    break;
  case 12:
    _TimeSeg1 = CAN_BS1_12tq;
    break;
  case 13:
    _TimeSeg1 = CAN_BS1_13tq;
    break;
  case 14:
    _TimeSeg1 = CAN_BS1_14tq;
    break;
  case 15:
    _TimeSeg1 = CAN_BS1_15tq;
    break;
  case 16:
    _TimeSeg1 = CAN_BS1_16tq;
    break;
  default:
    // should not happen
    _TimeSeg1 = CAN_BS1_1tq;
    break;
  }

  switch (timeseg2)
  {
  case 1:
    _TimeSeg2 = CAN_BS2_1tq;
    break;
  case 2:
    _TimeSeg2 = CAN_BS2_2tq;
    break;
  case 3:
    _TimeSeg2 = CAN_BS2_3tq;
    break;
  case 4:
    _TimeSeg2 = CAN_BS2_4tq;
    break;
  case 5:
    _TimeSeg2 = CAN_BS2_5tq;
    break;
  case 6:
    _TimeSeg2 = CAN_BS2_6tq;
    break;
  case 7:
    _TimeSeg2 = CAN_BS2_7tq;
    break;
  case 8:
    _TimeSeg2 = CAN_BS2_8tq;
    break;
  default:
    // should not happen
    _TimeSeg2 = CAN_BS2_1tq;
    break;
  }

  _Prescaler = prescaler;
}

size_t ZBCan::_allocateCanTxBuffer(size_t length)
{
  size_t ret = length;

  // Check if requested length exceeds maximum
  if (length > ZBCAN_MAX_BUFFER_SIZE)
  {
    ret = 0;
  }
  // Only reallocate if we need more space
  else if (_canTxBufferAllocated < length)
  {
    // Allocate at least CAN_MAX_FRAME_SIZE bytes
    if (length < ZBCAN_MAX_FRAME_SIZE)
    {
      length = ZBCAN_MAX_FRAME_SIZE;
    }

    uint8_t *tmp = (uint8_t *)realloc(_canTxBuffer, length * sizeof(uint8_t));
    if (tmp != nullptr)
    {
      _canTxBuffer = tmp;
      _canTxBufferAllocated = length;
    }
    else
    {
      ret = 0;
    }
  }

  return ret;
}

// Updated to include message type in the metadata
uint8_t ZBCan::_sendFrame(uint8_t *data, uint8_t len, uint32_t frameId, uint8_t frameIndex, bool isLastFrame, uint8_t type)
{
  uint8_t mbox;
  uint16_t i = 0;
  uint8_t status = 0;

  CanTxMsg CanTxStructure;

  if (_frameFormat == ZBCAN_STANDARD_FRAME)
  {
    CanTxStructure.StdId = frameId;
    CanTxStructure.IDE = CAN_Id_Standard;
  }
  else
  {
    CanTxStructure.ExtId = frameId;
    CanTxStructure.IDE = CAN_Id_Extended;
  }

  CanTxStructure.RTR = CAN_RTR_Data;

  // First byte contains frame metadata
  // Bit 7: Last frame flag (1 = last frame)
  // Bits 0-6: Frame index (0-127)
  uint8_t metadata = frameIndex & 0x7F;
  if (isLastFrame)
  {
    metadata |= 0x80; // Set the high bit if this is the last frame
  }

  // Second byte contains message type (for first frame only)
  // Maximum payload per frame is now 6 (8 bytes total with 2 metadata bytes)
  uint8_t maxPayload = isLastFrame && frameIndex == 0 ? 6 : 6;
  
  // Set data length (payload + 2 bytes for metadata)
  CanTxStructure.DLC = (len > maxPayload ? maxPayload : len) + (frameIndex == 0 ? 2 : 1);

  // Set metadata as first byte
  CanTxStructure.Data[0] = metadata;
  
  // Set type as second byte for first frame
  if (frameIndex == 0) {
    CanTxStructure.Data[1] = type;
  }

  // Copy payload data
  for (i = 0; i < len && i < maxPayload; i++)
  {
    CanTxStructure.Data[i + (frameIndex == 0 ? 2 : 1)] = data[i];
  }

  // Transmit the frame
  mbox = CAN_Transmit(CAN1, &CanTxStructure);
  i = 0;

  // Wait for transmission to complete
  while ((CAN_TransmitStatus(CAN1, mbox) != CAN_TxStatus_Ok) && (i < 0xFFF))
  {
    i++;
  }

  if (i == 0xFFF)
  {
    status = 1; // Transmission failed
  }
  
  // Add a tiny delay between frames to avoid bus saturation
  // Only do this for multi-frame messages (frameIndex > 0 or !isLastFrame)
  if (frameIndex > 0 || !isLastFrame) {
    // After each frame transmission, add a small variable delay
    // The delay increases for higher frame indices to prevent one device
    // from dominating the bus during multi-frame messages
    for (volatile uint16_t j = 0; j < (500 + frameIndex * 200); j++) {
      asm("nop");
    }
  }

  return status;
}

size_t ZBCan::write(const uint8_t *data, size_t size)
{
  size_t ret = size;

  // Allocate buffer space
  if (_allocateCanTxBuffer(_canTxBufferSize + size) == 0)
  {
    ret = 0;  // Fixed the typo here
  }
  else
  {
    // Copy data to buffer
    memcpy(&(_canTxBuffer[_canTxBufferSize]), data, size);

    // Update buffer size
    _canTxBufferSize += size;
  }

  return ret;
}

// Updated to include message type
uint8_t ZBCan::endTransmission(uint32_t id, uint8_t type)
{
  uint8_t status = 0;
  uint8_t frameIndex = 0;
  size_t bytesSent = 0;

  // If no data to send
  if (_canTxBufferSize == 0)
  {
    return 0;
  }

  // Use provided ID or current txId
  uint32_t frameId = (id != 0) ? id : _txCanId;

  // Send data in frames (maximum 6 bytes per frame due to metadata bytes)
  while (bytesSent < _canTxBufferSize)
  {
    // Calculate remaining bytes
    size_t remaining = _canTxBufferSize - bytesSent;

    // Maximum payload per frame is 6 (8 bytes total with metadata)
    // First frame has message type, so it can carry 1 byte less
    size_t payloadSize = frameIndex == 0 ? 6 : 6;
    if (remaining < payloadSize) {
      payloadSize = remaining;
    }

    // Check if this is the last frame
    bool isLastFrame = (bytesSent + payloadSize >= _canTxBufferSize);

    if (_sendFrame(&_canTxBuffer[bytesSent], payloadSize, frameId, frameIndex, isLastFrame, type))
    {
      status = 1;
      break;
    }

    // Update counters
    bytesSent += payloadSize;
    frameIndex++;
  }

  // Reset buffer
  _canTxBufferSize = 0;

  return status;
}

// Updated to extract message type
uint8_t ZBCan::_processFrame(uint8_t *data, uint8_t length, uint32_t canId)
{
  // Extract metadata from first byte
  uint8_t metadata = data[0];
  uint8_t frameIndex = metadata & 0x7F;      // Lower 7 bits = frame index
  bool isLastFrame = (metadata & 0x80) != 0; // High bit = last frame flag

  // Extract message type from second byte of first frame
  if (frameIndex == 0 && length >= 2) {
    _rxMsgType = data[1];
  }

  // If this is a new message start (frameIndex 0) but we have an incomplete message,
  // reset the receiver to handle the new message
  if (frameIndex == 0 && _rxBufferSize > 0 && _highestFrameIndex > 0) {
    resetReceiver();
  }

  // Sanity check for frame index
  if (frameIndex >= ZBCAN_MAX_EXPECTED_FRAMES)
  {
    return 0;
  }

  // Check if this frame has already been received
  if (_rxFramesReceived[frameIndex])
  {
    return 0;
  }

  // Check for out-of-sequence frames (more than one ahead)
  if (frameIndex > 0 && frameIndex > _highestFrameIndex + 1) {
    // Out of sequence - reset and ignore this frame
    resetReceiver();
    return 0;
  }

  // Check if CAN ID changed during multi-frame message
  if (_rxBufferSize > 0 && canId != _rxCanId) {
    // ID changed during multi-frame message - reset and start fresh
    resetReceiver();
    _rxCanId = canId;
  } else {
    // Store the CAN ID for this message
    _rxCanId = canId;
  }

  // Calculate payload size (excluding metadata bytes)
  uint8_t payloadSize = length - (frameIndex == 0 ? 2 : 1);

  // Calculate position in buffer
  // For frame 0, payload starts at data[2]
  // For frames > 0, payload starts at data[1], and we need to take into account the reduced
  // capacity of the first frame (6 bytes instead of 6)
  size_t bufferPos = frameIndex == 0 ? 0 : 6 + (frameIndex - 1) * 6;

  // Make sure we don't exceed buffer size
  if (bufferPos + payloadSize > ZBCAN_MAX_BUFFER_SIZE)
  {
    resetReceiver();
    return 0;
  }

  // Copy payload data to buffer (skip metadata bytes)
  memcpy(&_rxBuffer[bufferPos], &data[frameIndex == 0 ? 2 : 1], payloadSize);

  // Mark this frame as received
  _rxFramesReceived[frameIndex] = 1;

  // Update highest frame index
  if (frameIndex > _highestFrameIndex)
  {
    _highestFrameIndex = frameIndex;
  }

  // Update buffer size if needed
  size_t newSize = bufferPos + payloadSize;
  if (newSize > _rxBufferSize)
  {
    _rxBufferSize = newSize;
  }

  // If this is the last frame, check if we have all frames
  if (isLastFrame)
  {
    bool allFramesReceived = true;

    // Check if we have all frames from 0 to highest
    for (uint8_t i = 0; i <= _highestFrameIndex; i++)
    {
      if (!_rxFramesReceived[i])
      {
        allFramesReceived = false;
        break;
      }
    }

    if (allFramesReceived)
    {
      // Add null terminator for string messages
      if (_rxMsgType == ZBCAN_TYPE_STRING) {
        _rxBuffer[_rxBufferSize] = 0;
      }
      
      // Add message to queue and reset receiver for the next message
      if (enqueueMessage(_rxBuffer, _rxBufferSize, canId, _rxMsgType)) {
        // If callbacks are enabled and a callback function is registered,
        // process the message right away via appropriate callbacks
        if (_callbacksEnabled) {
          processCallbacks();
        }
        
        // Reset receiver for next message
        resetReceiver();
        return 1;
      }
    }
  }

  return 0;
}

// Updated to include message type
bool ZBCan::enqueueMessage(uint8_t* data, size_t length, uint32_t id, uint8_t type) 
{
  // Check if queue is full
  if (_queueCount >= ZBCAN_MESSAGE_QUEUE_SIZE) {
    // If queue is full, remove oldest message to make room
    if (_queueCount > 0) {
      _msgQueue[_queueHead].valid = false;
      _queueHead = (_queueHead + 1) % ZBCAN_MESSAGE_QUEUE_SIZE;
      _queueCount--;
    } else {
      return false;
    }
  }
  
  // Copy message to queue at tail position
  memcpy(_msgQueue[_queueTail].data, data, length);
  if (type == ZBCAN_TYPE_STRING) {
    _msgQueue[_queueTail].data[length] = 0; // Null terminator for strings
  }
  _msgQueue[_queueTail].length = length;
  _msgQueue[_queueTail].id = id;
  _msgQueue[_queueTail].type = type;
  _msgQueue[_queueTail].valid = true;
  
  // Update tail position
  _queueTail = (_queueTail + 1) % ZBCAN_MESSAGE_QUEUE_SIZE;
  
  // Increment message count
  _queueCount++;
  
  return true;
}

void ZBCan::resetReceiver(void)
{
  memset(_rxBuffer, 0, ZBCAN_MAX_BUFFER_SIZE);
  memset(_rxFramesReceived, 0, ZBCAN_MAX_EXPECTED_FRAMES);
  _rxBufferSize = 0;
  _messageComplete = false;
  _highestFrameIndex = 0;
  _rxMsgType = ZBCAN_TYPE_STRING; // Default to string type
}

bool ZBCan::messageAvailable()
{
  // Check if there are bus errors that need recovery
  checkBusStatus();
  
  return (_queueCount > 0);
}

// Updated to return message type
size_t ZBCan::receiveMessage(uint8_t* buffer, uint8_t* type)
{
  // If no message available
  if (_queueCount == 0) {
    return 0;
  }

  // Get message from queue at head position
  memcpy(buffer, _msgQueue[_queueHead].data, _msgQueue[_queueHead].length);
  
  // If this is a string, add null terminator
  if (_msgQueue[_queueHead].type == ZBCAN_TYPE_STRING) {
    buffer[_msgQueue[_queueHead].length] = 0;
  }
  
  _rxCanId = _msgQueue[_queueHead].id; // Update the current ID
  _lastMsgType = _msgQueue[_queueHead].type; // Store the message type
  
  if (type != nullptr) {
    *type = _lastMsgType; // Return the message type if requested
  }
  
  size_t msgLength = _msgQueue[_queueHead].length;
  
  // Mark message as invalid
  _msgQueue[_queueHead].valid = false;
  
  // Update head position
  _queueHead = (_queueHead + 1) % ZBCAN_MESSAGE_QUEUE_SIZE;
  
  // Decrement message count
  _queueCount--;

  return msgLength;
}

bool ZBCan::checkBusStatus()
{
  // If not initialized, no need to check
  if (!_isInitialized) {
    return false;
  }
  
  // Check CAN status register for errors
  uint32_t errorFlags = CAN1->ERRSR;
  bool hasErrors = (errorFlags & (CAN_ERRSR_BOFF | CAN_ERRSR_EPVF)) != 0;
  
  if (hasErrors) {
    _errorCount++;
    
    // Check for bus-off condition
    if (errorFlags & CAN_ERRSR_BOFF) {
      _busOffState = true;
      // Immediate recovery attempt for bus-off
      recoverFromErrors();
      return false;
    }
    
    // If error count is high, try to recover
    if (_errorCount > 3) {
      recoverFromErrors();
      return false;
    }
  } else {
    // Reset error count after some successful operations
    if (_errorCount > 0 && (_errorCount % 10) == 0) {
      _errorCount--;
    }
  }
  
  return !_busOffState;
}

void ZBCan::recoverFromErrors()
{
  // Only attempt recovery if we're initialized
  if (!_isInitialized) {
    return;
  }
  
  // Reset the CAN peripheral
  CAN_DeInit(CAN1);
  
  // Small delay to allow bus to stabilize
  for (volatile uint16_t i = 0; i < 10000; i++) {
    asm("nop");
  }
  
  // Re-initialize with saved parameters
  reinitialize();
  
  // Reset error tracking
  _errorCount = 0;
  _busOffState = false;
}

void ZBCan::reinitialize()
{
  // Reinitialize with saved parameters
  beginManual(_SyncJumpWidth, _TimeSeg2, _TimeSeg1, _Prescaler, _mode, _txCanId, _frameFormat);
  
  // Reset receiver state
  resetReceiver();
}

void ZBCan::processInterruptFrame(CanRxMsg* rxMsg)
{
  // Extract CAN ID for early checks
  uint32_t canId = (rxMsg->IDE == CAN_Id_Standard) ? rxMsg->StdId : rxMsg->ExtId;
  
  // Extract metadata from first byte for early checks
  uint8_t frameIndex = rxMsg->Data[0] & 0x7F;  // Lower 7 bits = frame index
  
  // If this is the first frame of a new message (index 0) but we have a partial message,
  // reset the receiver to start fresh
  if (frameIndex == 0 && _rxBufferSize > 0) {
    resetReceiver();
  }
  
  // Check if queue is almost full - if so, make room
  if (_queueCount >= ZBCAN_MESSAGE_QUEUE_SIZE - 1) {
    // Queue getting full, discard oldest message
    if (_queueCount > 0) {
      _queueHead = (_queueHead + 1) % ZBCAN_MESSAGE_QUEUE_SIZE;
      _queueCount--;
    }
  }

  // Process the received frame
  _processFrame(rxMsg->Data, rxMsg->DLC, canId);
}

uint8_t ZBCan::getSJW(void) {
  // Convert the CAN_SJW_xtq constants back to numeric values
  switch (_SyncJumpWidth) {
    case CAN_SJW_1tq:
      return 1;
    case CAN_SJW_2tq:
      return 2;
    case CAN_SJW_3tq:
      return 3;
    case CAN_SJW_4tq:
      return 4;
    default:
      return 0; // Invalid value
  }
}

uint8_t ZBCan::getTimeSeg1(void) {
  // Convert the CAN_BS1_xtq constants back to numeric values
  switch (_TimeSeg1) {
    case CAN_BS1_1tq:
      return 1;
    case CAN_BS1_2tq:
      return 2;
    case CAN_BS1_3tq:
      return 3;
    case CAN_BS1_4tq:
      return 4;
    case CAN_BS1_5tq:
      return 5;
    case CAN_BS1_6tq:
      return 6;
    case CAN_BS1_7tq:
      return 7;
    case CAN_BS1_8tq:
      return 8;
    case CAN_BS1_9tq:
      return 9;
    case CAN_BS1_10tq:
      return 10;
    case CAN_BS1_11tq:
      return 11;
    case CAN_BS1_12tq:
      return 12;
    case CAN_BS1_13tq:
      return 13;
    case CAN_BS1_14tq:
      return 14;
    case CAN_BS1_15tq:
      return 15;
    case CAN_BS1_16tq:
      return 16;
    default:
      return 0; // Invalid value
  }
}

uint8_t ZBCan::getTimeSeg2(void) {
  // Convert the CAN_BS2_xtq constants back to numeric values
  switch (_TimeSeg2) {
    case CAN_BS2_1tq:
      return 1;
    case CAN_BS2_2tq:
      return 2;
    case CAN_BS2_3tq:
      return 3;
    case CAN_BS2_4tq:
      return 4;
    case CAN_BS2_5tq:
      return 5;
    case CAN_BS2_6tq:
      return 6;
    case CAN_BS2_7tq:
      return 7;
    case CAN_BS2_8tq:
      return 8;
    default:
      return 0; // Invalid value
  }
}

uint16_t ZBCan::getPrescaler(void) {
  return _Prescaler;
}

uint32_t ZBCan::getCurrentBaudRate(void) {
  // If any parameter is not set, return 0
  if (_Prescaler == 0 || getTimeSeg1() == 0 || getTimeSeg2() == 0) {
    return 0;
  }

  // Get the APB1 clock frequency
  RCC_ClocksTypeDef RCC_ClocksStatus;
  RCC_GetClocksFreq(&RCC_ClocksStatus);
  uint32_t pclk1 = RCC_ClocksStatus.PCLK1_Frequency;
  
  // Calculate BaudRate = pclk1 / ((_Prescaler) * (1 + getTimeSeg1() + getTimeSeg2()))
  uint32_t baudRate = pclk1 / (_Prescaler * (1 + getTimeSeg1() + getTimeSeg2()));
  
  return baudRate;
}

// Interrupt handler for CAN1 RX1
extern "C" {
  void CAN1_RX1_IRQHandler(void) 
  {
    // Check for FIFO overrun
    if (CAN_GetITStatus(CAN1, CAN_IT_FOV1) != RESET) 
    {
      // Clear FIFO overrun flag
      CAN_ClearITPendingBit(CAN1, CAN_IT_FOV1);
      
      // Reset receiver state if we have an active instance
      if (activeCANInstance != nullptr) {
        activeCANInstance->resetReceiver();
      }
    }
    
    // Check for message pending in FIFO1
    if (CAN_GetITStatus(CAN1, CAN_IT_FMP1) != RESET) 
    {
      // Get message from FIFO1
      CanRxMsg rxMsg;
      CAN_Receive(CAN1, CAN_FIFO1, &rxMsg);
      
      // Process the message if we have an active instance
      if (activeCANInstance != nullptr) {
        activeCANInstance->processInterruptFrame(&rxMsg);
      }
      
      // Clear interrupt flag
      CAN_ClearITPendingBit(CAN1, CAN_IT_FMP1);
    }
    
    // Handle all CAN error flags here
    if (CAN_GetITStatus(CAN1, CAN_IT_BOF | CAN_IT_EPV | CAN_IT_EWG) != RESET) 
    {
      // Clear all error flags
      CAN_ClearITPendingBit(CAN1, CAN_IT_BOF | CAN_IT_EPV | CAN_IT_EWG);
      
      // Try to recover from bus error if we have an active instance
      if (activeCANInstance != nullptr && (CAN1->ERRSR & CAN_ERRSR_BOFF)) {
        activeCANInstance->recoverFromErrors();
      }
    }
  }
} // End of extern "C" block
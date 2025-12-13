#include "HexParser.h"

// Static member initialization
HexParser* HexParser::instance = nullptr;

HexParser::HexParser(uint32_t buffer_size)
    : buffer_size(buffer_size), flash_size(0) {
  flash_buffer = new uint8_t[buffer_size];
  if (!flash_buffer) {
    Serial.println("Failed to allocate flash buffer");
    return;
  }
  clearBuffer();
  instance = this;  // Set static instance for callback
}

HexParser::~HexParser() {
  if (flash_buffer) {
    delete[] flash_buffer;
  }
  if (instance == this) {
    instance = nullptr;
  }
}

void HexParser::clearBuffer() {
  if (flash_buffer) {
    memset(flash_buffer, 0xFF, buffer_size);
    flash_size = 0;
  }
}

bool HexParser::parseFile(File& file) {

  if (!flash_buffer) {
    Serial.println("Flash buffer not allocated");
    return false;
  }

  if (!file) {
    Serial.print("Failed to open file");
    return false;
  }

  Serial.printf("Parsing HEX file: %s (%d bytes)\n", file.name(), file.size());

  // Initialize HEX parser
  ihex_begin_read(&ihex_state);
  clearBuffer();

  // Read and parse file in chunks
  char buffer[256];
  bool parse_success = true;

  while (file.available() && parse_success) {
    size_t bytes_read = file.readBytes(buffer, sizeof(buffer) - 1);
    buffer[bytes_read] = '\0';

    if (!ihex_read_bytes(&ihex_state, buffer, bytes_read)) {
      Serial.println("HEX parsing error");
      parse_success = false;
    }
  }

  ihex_end_read(&ihex_state);
  file.close();

  if (parse_success) {
    Serial.printf("HEX file parsed successfully. Flash size: %d bytes\n",
                   flash_size);
    printParseInfo();
  }

  return parse_success;
}

void HexParser::printParseInfo() const {
  Serial.printf("Parsed HEX Info:\n");
  Serial.printf("  Buffer size: %d bytes\n", buffer_size);
  Serial.printf("  Flash size: %d bytes\n", flash_size);
  Serial.printf("  Usage: %.1f%%\n", (float)flash_size * 100.0 / buffer_size);

  // Find first and last non-0xFF bytes
  uint32_t first_byte = 0, last_byte = 0;
  for (uint32_t i = 0; i < flash_size; i++) {
    if (flash_buffer[i] != 0xFF) {
      first_byte = i;
      break;
    }
  }
  for (uint32_t i = flash_size; i > 0; i--) {
    if (flash_buffer[i - 1] != 0xFF) {
      last_byte = i - 1;
      break;
    }
  }

  if (first_byte <= last_byte) {
    Serial.printf("  Code range: 0x%04X - 0x%04X\n", first_byte, last_byte);
  }
}

// Static callback method for global callback function
ihex_bool_t HexParser::ihex_data_callback(struct ihex_state* ihex,
                                          ihex_record_type_t type,
                                          ihex_bool_t checksum_error) {
  if (instance) {
    return instance->handleParsedData(ihex, type, checksum_error);
  }
  return false;
}

// Instance method for handling parsed data
ihex_bool_t HexParser::handleParsedData(struct ihex_state* ihex,
                                        ihex_record_type_t type,
                                        ihex_bool_t checksum_error) {
  if (checksum_error) {
    Serial.println("HEX checksum error!");
    return false;
  }

  switch (type) {
    case IHEX_DATA_RECORD: {
      uint32_t address = IHEX_LINEAR_ADDRESS(ihex);

      // Check if address is within flash range
      if (address + ihex->length > buffer_size) {
        Serial.printf("Address 0x%08X exceeds buffer size\n", address);
        return false;
      }

      // Copy data to flash buffer
      memcpy(&flash_buffer[address], ihex->data, ihex->length);

      // Update flash size
      if (address + ihex->length > flash_size) {
        flash_size = address + ihex->length;
      }

      Serial.printf("Data: 0x%08X, %d bytes\n", address, ihex->length);
      break;
    }

    case IHEX_END_OF_FILE_RECORD:
      Serial.println("HEX file parsing complete");
      break;

    case IHEX_EXTENDED_SEGMENT_ADDRESS_RECORD:
    case IHEX_START_SEGMENT_ADDRESS_RECORD:
    case IHEX_EXTENDED_LINEAR_ADDRESS_RECORD:
    case IHEX_START_LINEAR_ADDRESS_RECORD:
      // Handle address records - processed internally by parser
      break;

    default:
      Serial.printf("Unknown record type: %d\n", type);
      break;
  }

  return true;
}

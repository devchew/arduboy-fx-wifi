#include "HexParser.h"

// ==========================================
// IHEX CALLBACK FUNCTION
// ==========================================
// Global callback function required by kk_ihex library
extern "C" ihex_bool_t ihex_data_read(struct ihex_state* ihex,
                                      ihex_record_type_t type,
                                      ihex_bool_t checksum_error) {
  // Forward to HexParser static callback
  return HexParser::ihex_data_callback(ihex, type, checksum_error);
}

// Static member initialization
HexParser* HexParser::instance = nullptr;

HexParser::HexParser(uint32_t buffer_size)
    : buffer_size(buffer_size), flash_size(0) {
  flash_buffer = new uint8_t[buffer_size];
  if (!flash_buffer) {
    Logger::error("Failed to allocate flash buffer");
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
    Logger::error("Flash buffer not allocated");
    return false;
  }

  if (!file) {
    Logger::error("Failed to open file");
    return false;
  }

  Logger::info("Parsing HEX file: %s (%d bytes)\n", file.name(), file.size());

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
      Logger::error("HEX parsing error");
      parse_success = false;
    }
  }

  ihex_end_read(&ihex_state);
  file.close();

  if (parse_success) {
    Logger::info("HEX file parsed successfully. Flash size: %d bytes\n",
                   flash_size);
    printParseInfo();
  }

  return parse_success;
}

void HexParser::printParseInfo() const {
  Logger::info("Parsed HEX Info:\n");
  Logger::info("  Buffer size: %d bytes\n", buffer_size);
  Logger::info("  Flash size: %d bytes\n", flash_size);
  Logger::info("  Usage: %.1f%%\n", (float)flash_size * 100.0 / buffer_size);

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
    Logger::info("  Code range: 0x%04X - 0x%04X\n", first_byte, last_byte);
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
    Logger::error("HEX checksum error!");
    return false;
  }

  switch (type) {
    case IHEX_DATA_RECORD: {
      uint32_t address = IHEX_LINEAR_ADDRESS(ihex);

      // Check if address is within flash range
      if (address + ihex->length > buffer_size) {
        Logger::error("Address 0x%08X exceeds buffer size\n", address);
        return false;
      }

      // Copy data to flash buffer
      memcpy(&flash_buffer[address], ihex->data, ihex->length);

      // Update flash size
      if (address + ihex->length > flash_size) {
        flash_size = address + ihex->length;
      }

      Logger::info("Data: 0x%08X, %d bytes\n", address, ihex->length);
      break;
    }

    case IHEX_END_OF_FILE_RECORD:
      Logger::info("HEX file parsing complete");
      break;

    case IHEX_EXTENDED_SEGMENT_ADDRESS_RECORD:
    case IHEX_START_SEGMENT_ADDRESS_RECORD:
    case IHEX_EXTENDED_LINEAR_ADDRESS_RECORD:
    case IHEX_START_LINEAR_ADDRESS_RECORD:
      // Handle address records - processed internally by parser
      break;

    default:
      Logger::info("Unknown record type: %d\n", type);
      break;
  }

  return true;
}

// Apply a modifier callback to the internal flash buffer.
bool HexParser::modifyBuffer(HexParser::buffer_modifier_t modifier, void* ctx) {
  if (!flash_buffer || !modifier) return false;
  // modifier can inspect buffer_size and flash_size
  return modifier(flash_buffer, buffer_size, flash_size, ctx);
}

// Write the parser's internal flash buffer as Intel HEX to an Arduino File
bool HexParser::writeHexFile(File& file) const {
  if (!file) {
    Logger::error("Output file is not open");
    return false;
  }
  const uint32_t WRITE_RECORD_SIZE = 16;
  char line[128];

  uint32_t i = 0;
  while (i < buffer_size) {
    // skip 0xFF runs
    while (i < buffer_size && flash_buffer[i] == 0xFF) ++i;
    if (i >= buffer_size) break;
    uint32_t runStart = i;
    uint32_t runLen = 0;
    while (i < buffer_size && flash_buffer[i] != 0xFF && runLen < 0xFFFF) { ++i; ++runLen; }

    uint32_t written = 0;
    while (written < runLen) {
      uint32_t chunk = (runLen - written) > WRITE_RECORD_SIZE ? WRITE_RECORD_SIZE : (runLen - written);
      uint16_t addr = (uint16_t)(runStart + written);
      int hdrlen = snprintf(line, sizeof(line), ":%02X%04X%02X", (int)chunk, (int)addr, 0);
      if (hdrlen < 0) return false;
      size_t pos = (size_t)hdrlen;
      // append data bytes
      for (uint32_t k = 0; k < chunk; ++k) {
        int w = snprintf(line + pos, sizeof(line) - pos, "%02X", (int)flash_buffer[runStart + written + k]);
        if (w < 0) return false;
        pos += (size_t)w;
      }
      // compute checksum
      uint32_t sum = 0;
      sum += (uint8_t)chunk;
      sum += (uint8_t)((addr >> 8) & 0xFF);
      sum += (uint8_t)(addr & 0xFF);
      sum += 0x00; // record type
      for (uint32_t k = 0; k < chunk; ++k) sum += flash_buffer[runStart + written + k];
      uint8_t csum = (uint8_t)((~(sum & 0xFF) + 1) & 0xFF);
      int w = snprintf(line + pos, sizeof(line) - pos, "%02X", (int)csum);
      if (w < 0) return false;
      // write line
      file.print(line);
      file.println();
      written += chunk;
    }
  }
  // EOF
  file.println(":00000001FF");
  return true;
}

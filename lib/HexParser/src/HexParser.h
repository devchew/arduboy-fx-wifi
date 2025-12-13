#ifndef HEX_PARSER_H
#define HEX_PARSER_H

#include <Arduino.h>
#include <FS.h>

#include "kk_ihex_read.h"

class HexParser {
 private:
  uint8_t* flash_buffer;
  uint32_t buffer_size;
  uint32_t flash_size;
  struct ihex_state ihex_state;

  static HexParser* instance;  // For callback

  // Instance method for handling parsed data
  ihex_bool_t handleParsedData(struct ihex_state* ihex, ihex_record_type_t type,
                               ihex_bool_t checksum_error);

 public:
  HexParser(uint32_t buffer_size = 32768);
  ~HexParser();

  bool parseFile(File& file);

  uint8_t* getFlashBuffer() const { return flash_buffer; }
  uint32_t getFlashSize() const { return flash_size; }
  uint32_t getBufferSize() const { return buffer_size; }

  void clearBuffer();
  void printParseInfo() const;

  // Static callback method for global callback function
  static ihex_bool_t ihex_data_callback(struct ihex_state* ihex,
                                        ihex_record_type_t type,
                                        ihex_bool_t checksum_error);
};

#endif  // HEX_PARSER_H

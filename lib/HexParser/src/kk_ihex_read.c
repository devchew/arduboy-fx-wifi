#include "kk_ihex_read.h"
#include <ctype.h>

// External callback function declaration
extern ihex_bool_t ihex_data_read(struct ihex_state *ihex, ihex_record_type_t type, ihex_bool_t checksum_error);

static uint8_t hex_digit_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

static uint8_t read_hex_byte(const char *hex) {
    return (hex_digit_value(hex[0]) << 4) | hex_digit_value(hex[1]);
}

void ihex_begin_read(struct ihex_state *ihex) {
    ihex->address = 0;
    ihex->segment = 0;
    ihex->state = IHEX_READ_WHOLE_LINE;
    ihex->i = 0;
}

ihex_bool_t ihex_read_bytes(struct ihex_state *ihex, const char *data, size_t length) {
    for (size_t pos = 0; pos < length; pos++) {
        char c = data[pos];
        
        switch (ihex->state) {
            case IHEX_READ_WHOLE_LINE:
                if (c == ':') {
                    ihex->state = IHEX_READ_BYTE_COUNT;
                    ihex->i = 0;
                    ihex->checksum = 0;
                }
                break;
                
            case IHEX_READ_BYTE_COUNT:
                if (isxdigit(c) && ihex->i < 2) {
                    if (ihex->i == 0) {
                        ihex->length = hex_digit_value(c) << 4;
                    } else {
                        ihex->length |= hex_digit_value(c);
                        ihex->checksum = ihex->length;
                        ihex->state = IHEX_READ_ADDRESS_HIGH;
                        ihex->i = 0;
                        continue;
                    }
                    ihex->i++;
                }
                break;
                
            case IHEX_READ_ADDRESS_HIGH:
                if (isxdigit(c) && ihex->i < 2) {
                    if (ihex->i == 0) {
                        ihex->address_high = hex_digit_value(c) << 4;
                    } else {
                        ihex->address_high |= hex_digit_value(c);
                        ihex->checksum += ihex->address_high;
                        ihex->state = IHEX_READ_ADDRESS_LOW;
                        ihex->i = 0;
                        continue;
                    }
                    ihex->i++;
                }
                break;
                
            case IHEX_READ_ADDRESS_LOW:
                if (isxdigit(c) && ihex->i < 2) {
                    if (ihex->i == 0) {
                        ihex->address_low = hex_digit_value(c) << 4;
                    } else {
                        ihex->address_low |= hex_digit_value(c);
                        ihex->checksum += ihex->address_low;
                        ihex->address = (ihex->segment << 16) | (ihex->address_high << 8) | ihex->address_low;
                        ihex->state = IHEX_READ_TYPE;
                        ihex->i = 0;
                        continue;
                    }
                    ihex->i++;
                }
                break;
                
            case IHEX_READ_TYPE:
                if (isxdigit(c) && ihex->i < 2) {
                    if (ihex->i == 0) {
                        ihex->type = hex_digit_value(c) << 4;
                    } else {
                        ihex->type |= hex_digit_value(c);
                        ihex->checksum += ihex->type;
                        ihex->state = ihex->length > 0 ? IHEX_READ_DATA : IHEX_READ_CHECKSUM;
                        ihex->i = 0;
                        continue;
                    }
                    ihex->i++;
                }
                break;
                
            case IHEX_READ_DATA:
                if (isxdigit(c) && ihex->i < ihex->length * 2) {
                    uint8_t byte_idx = ihex->i / 2;
                    if (ihex->i % 2 == 0) {
                        ihex->data[byte_idx] = hex_digit_value(c) << 4;
                    } else {
                        ihex->data[byte_idx] |= hex_digit_value(c);
                        ihex->checksum += ihex->data[byte_idx];
                    }
                    ihex->i++;
                    if (ihex->i >= ihex->length * 2) {
                        ihex->state = IHEX_READ_CHECKSUM;
                        ihex->i = 0;
                    }
                }
                break;
                
            case IHEX_READ_CHECKSUM:
                if (isxdigit(c) && ihex->i < 2) {
                    if (ihex->i == 0) {
                        ihex->checksum = (ihex->checksum + (hex_digit_value(c) << 4));
                    } else {
                        ihex->checksum = (ihex->checksum + hex_digit_value(c)) & 0xFF;
                        
                        // Handle different record types
                        if (ihex->type == IHEX_EXTENDED_LINEAR_ADDRESS_RECORD && ihex->length == 2) {
                            ihex->segment = (ihex->data[0] << 8) | ihex->data[1];
                        }
                        
                        // Call user callback
                        if (!ihex_data_read(ihex, ihex->type, ihex->checksum != 0)) {
                            return false;
                        }
                        
                        ihex->state = IHEX_IGNORE_LF;
                        ihex->i = 0;
                        continue;
                    }
                    ihex->i++;
                }
                break;
                
            case IHEX_IGNORE_LF:
                if (c == '\n' || c == '\r') {
                    ihex->state = IHEX_READ_WHOLE_LINE;
                } else if (c == ':') {
                    ihex->state = IHEX_READ_BYTE_COUNT;
                    ihex->i = 0;
                    ihex->checksum = 0;
                }
                break;
        }
    }
    
    return true;
}

void ihex_end_read(struct ihex_state *ihex) {
    // Nothing to do
    (void)ihex;
}

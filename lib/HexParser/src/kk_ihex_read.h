#ifndef KK_IHEX_READ_H
#define KK_IHEX_READ_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bool ihex_bool_t;
typedef uint8_t ihex_record_type_t;

#define IHEX_DATA_RECORD                    0
#define IHEX_END_OF_FILE_RECORD             1
#define IHEX_EXTENDED_SEGMENT_ADDRESS_RECORD 2
#define IHEX_START_SEGMENT_ADDRESS_RECORD   3
#define IHEX_EXTENDED_LINEAR_ADDRESS_RECORD 4
#define IHEX_START_LINEAR_ADDRESS_RECORD    5

struct ihex_state {
    uint32_t address;
    uint32_t segment;
    uint8_t data[255];
    uint8_t length;
    uint8_t line_length;
    uint8_t address_high;
    uint8_t address_low;
    uint8_t type;
    uint8_t checksum;
    enum {
        IHEX_READ_WHOLE_LINE,
        IHEX_READ_BYTE_COUNT,
        IHEX_READ_ADDRESS_HIGH,
        IHEX_READ_ADDRESS_LOW,
        IHEX_READ_TYPE,
        IHEX_READ_DATA,
        IHEX_READ_CHECKSUM,
        IHEX_IGNORE_LF
    } state;
    uint8_t i;
};

#define IHEX_LINEAR_ADDRESS(ihex) ((ihex)->address)

void ihex_begin_read(struct ihex_state *ihex);
ihex_bool_t ihex_read_bytes(struct ihex_state *ihex, const char *data, size_t length);
void ihex_end_read(struct ihex_state *ihex);

// Callback function to be implemented by user
ihex_bool_t ihex_data_read(struct ihex_state *ihex, ihex_record_type_t type, ihex_bool_t checksum_error);

#ifdef __cplusplus
}
#endif

#endif

#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// SERIAL ALIAS CONFIGURATION
// ==========================================
// Create alias for Serial to use Serial0 (native USB port on ESP32-S2)
// #define Serial Serial0

// ==========================================
// SHARED SPI CONFIGURATION (Default VSPI/SPI3)
// ==========================================
// ESP32-S2 Default SPI pins (VSPI/SPI3) - Shared by OLED and Arduboy ISP
// SCK  = GPIO7  (Default VSPI SCK)
// MOSI = GPIO11 (Default VSPI MOSI)
// MISO = GPIO9  (Default VSPI MISO)
// Note: These are the default pins, no need to define them explicitly

// ==========================================
// ISP PIN CONFIGURATION
// ==========================================
// ISP pin definitions for ESP32-S2 to ATmega32U4 (Arduboy)
#define ISP_RESET_PIN 21  // GPIO15 (custom pin for reset)
// Uses default SPI pins (GPIO7=SCK, GPIO11=MOSI, GPIO9=MISO)

// ==========================================
// OLED CONFIGURATION
// ==========================================
#define OLED_RESET_PIN 18  // GPIO18 (custom pin for OLED reset)
#define OLED_DC_PIN 16     // GPIO17 (custom pin for OLED DC)
#define OLED_CS_PIN 17     // GPIO16 (custom pin for OLED CS)
// Uses default SPI pins (GPIO7=SCK, GPIO11=MOSI)


// ==========================================
// WIFI CONFIGURATION (Optional)
// ==========================================
// WiFi credentials for remote programming features
#define WIFI_ENABLED false
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// ==========================================
// HEX PARSER CONFIGURATION
// ==========================================
// Flash buffer size for ATmega32U4 (Arduboy)
#define HEX_BUFFER_SIZE 32768  // 32KB for ATmega32U4

// Default HEX file path
#define DEFAULT_HEX_PATH "/firmware.hex"

// ==========================================
// SERIAL CONFIGURATION
// ==========================================
#define SERIAL_BAUD_RATE 115200

// ==========================================
// FILESYSTEM CONFIGURATION
// ==========================================
// Enable automatic filesystem formatting if mount fails
#define AUTO_FORMAT_FS false

// ==========================================
// PROGRAMMING CONFIGURATION
// ==========================================
// Enable verbose programming output
#define VERBOSE_PROGRAMMING true

// Programming delays (milliseconds)
#define CHIP_ERASE_DELAY 100
#define PAGE_WRITE_DELAY 10

// Progress update frequency
#define PROGRESS_UPDATE_PAGES 10    // Show progress every N pages
#define PROGRESS_UPDATE_BYTES 1024  // Show verify progress every N bytes

#endif  // CONFIG_H

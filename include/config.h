#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// ISP PIN CONFIGURATION
// ==========================================
// ISP pin definitions for ESP32-C3 to ATmega32U4 (Arduboy)
#define ISP_RESET_PIN 3      // GPIO3 (custom pin for reset)
#define POWER_CONTROL_PIN 2  // GPIO2 (power control for Arduboy)
// Using default ESP32-C3 hardware SPI pins:
// SCK  = GPIO6 (default)
// MOSI = GPIO7 (default)
// MISO = GPIO5 (default)

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

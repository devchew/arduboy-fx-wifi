#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// ISP PIN CONFIGURATION
// ==========================================
// ISP pin definitions for ESP8266 (adjust based on your wiring)
#define ISP_RESET_PIN 3    // D1 on ESP8266
#define ISP_SCK_PIN SCK    // D5 on ESP8266
#define ISP_MOSI_PIN MISO  // D7 on ESP8266
#define ISP_MISO_PIN MOSI  // D6 on ESP8266

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
// Flash buffer size for storing parsed HEX data
#define HEX_BUFFER_SIZE 32768  // 32KB for ATmega32U4

// Default HEX file path (can be overridden via commands)
#define DEFAULT_HEX_PATH "/firmware.hex"

// ==========================================
// SERIAL CONFIGURATION
// ==========================================
#define SERIAL_BAUD_RATE 115200

// ==========================================
// FILESYSTEM CONFIGURATION
// ==========================================
// Enable automatic filesystem formatting if mount fails
#define AUTO_FORMAT_FS true

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

// ==========================================
// DEVICE SUPPORT
// ==========================================
// Target device configuration
#define DEFAULT_TARGET_DEVICE "ATmega32U4"

// Enable support for multiple device types
#define SUPPORT_ATMEGA328P true
#define SUPPORT_ATMEGA32U4 true

#endif  // CONFIG_H

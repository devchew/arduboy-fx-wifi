#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// SERIAL CONFIGURATION
// ==========================================
#define SERIAL_BAUD_RATE  115200
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
// ISP PROGRAMMER CONFIGURATION
// ==========================================
// ISP pin definitions for ESP32-S2 to ATmega32U4 (Arduboy)
#define ISP_RESET_PIN     21  // GPIO15 (custom pin for reset)
// Uses default SPI pins (GPIO7=SCK, GPIO11=MOSI, GPIO9=MISO)
// Flash buffer size for ATmega32U4 (Arduboy)
#define HEX_BUFFER_SIZE   32768  // 32KB for ATmega32U4

// ==========================================
// OLED CONFIGURATION
// ==========================================
#define U8G2_SCREEN U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI
#define OLED_RESET_PIN    18  // GPIO18 (custom pin for OLED reset)
#define OLED_DC_PIN       16     // GPIO17 (custom pin for OLED DC)
#define OLED_CS_PIN       17     // GPIO16 (custom pin for OLED CS)
// Uses default SPI pins (GPIO7=SCK, GPIO11=MOSI)

// ==========================================
// SD CONFIGURATION
// ==========================================
#define SD_CS_PIN        1
#define SD_MOSI_PIN      2
#define SD_MISO_PIN      4
#define SD_SCK_PIN       3

// ==========================================
// Buttons pins
// ==========================================
#define BUTTON_PIN_UP     36 //34
#define BUTTON_PIN_DOWN   33 //35
#define BUTTON_PIN_LEFT   34 //36
#define BUTTON_PIN_RIGHT  35 //33
#define BUTTON_PIN_A      40
#define BUTTON_PIN_B      39
#define BUTTON_PIN_SELECT 38
#define BUTTON_PIN_START  37


// ==========================================
// WIFI CONFIGURATION (Optional)
// ==========================================
// WiFi credentials for remote programming features
#define WIFI_ENABLED      false
#define WIFI_SSID         "your_wifi_ssid"
#define WIFI_PASSWORD     "your_wifi_password"


#endif  // CONFIG_H

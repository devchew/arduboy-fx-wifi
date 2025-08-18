#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <LittleFS.h>

#include "FS.h"
#include "HexParser.h"
#include "ISPProgrammer.h"
#include "config.h"

// ==========================================
// GLOBAL OBJECTS
// ==========================================
HexParser* hexParser = nullptr;
ISPProgrammer* ispProgrammer = nullptr;

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

// ==========================================
// UTILITY FUNCTIONS
// ==========================================
void listDir(fs::FS& fs, const char* dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");

#ifdef CONFIG_LITTLEFS_FOR_IDF_3_2
      Serial.println(file.name());
#else
      Serial.print(file.name());
      time_t t = file.getLastWrite();
      struct tm* tmstruct = localtime(&t);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",
                    (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1,
                    tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min,
                    tmstruct->tm_sec);
#endif

      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");

#ifdef CONFIG_LITTLEFS_FOR_IDF_3_2
      Serial.println(file.size());
#else
      Serial.print(file.size());
      time_t t = file.getLastWrite();
      struct tm* tmstruct = localtime(&t);
      Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",
                    (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1,
                    tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min,
                    tmstruct->tm_sec);
#endif
    }
    file = root.openNextFile();
  }
}

void listFiles() {
  Serial.println("Files in LittleFS:");
  listDir(LittleFS, "/", 1);
}

void showDeviceInfo() {
  if (!ispProgrammer) {
    Serial.println("ISP programmer not initialized");
    return;
  }

  if (ispProgrammer->begin()) {
    if (ispProgrammer->enterProgrammingMode()) {
      ispProgrammer->printDeviceInfo();
      ispProgrammer->printFuses();
    }
    ispProgrammer->exitProgrammingMode();
    ispProgrammer->end();
  }
}

void showHelp() {
  Serial.println("Commands:");
  Serial.println("  program <filename>  - Program HEX file to AVR");
  Serial.println("  list               - List files in LittleFS");
  Serial.println("  info               - Show AVR device info");
  Serial.println("  help               - Show this help");
  Serial.println();
  Serial.println("Example: program firmware.hex");
}

void printWelcome() {
  Serial.println();
  Serial.println("ESP AVR ISP Programmer");
  Serial.println("==========================");
  Serial.printf("Target: %s\n", DEFAULT_TARGET_DEVICE);
  Serial.println();
}

void printConnections() {
  Serial.println("ISP Connections (ESP -> ATmega32U4):");
  Serial.printf("  GPIO%d  -> RESET\n", ISP_RESET_PIN);
  Serial.printf("  GPIO%d  -> SCK\n", ISP_SCK_PIN);
  Serial.printf("  GPIO%d  -> MOSI\n", ISP_MOSI_PIN);
  Serial.printf("  GPIO%d  -> MISO\n", ISP_MISO_PIN);
  Serial.println("  GND     -> GND");
  Serial.println("  3V3     -> VCC (if powering target)");
  Serial.println();
}

bool initializeFilesystem() {
  if (!LittleFS.begin(AUTO_FORMAT_FS)) {
    Serial.println("Failed to mount LittleFS");
    return false;
  }

  listFiles();

  Serial.println("LittleFS mounted successfully");
  return true;
}

void initializeWiFi() {
  if (WIFI_ENABLED) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println();
      Serial.println("WiFi connection failed, continuing without WiFi");
    }
  }
}

// ==========================================
// COMMAND HANDLING
// ==========================================
bool programHexFile(const String& filename) {
  if (!hexParser || !ispProgrammer) {
    Serial.println("Parser or programmer not initialized");
    return false;
  }

  String fullPath = filename;
  if (!fullPath.startsWith("/")) {
    fullPath = "/" + fullPath;
  }

  Serial.printf("Programming file: %s\n", fullPath.c_str());

  // Parse HEX file
  if (!hexParser->parseFile(fullPath)) {
    Serial.println("Failed to parse HEX file");
    return false;
  }

  // Initialize ISP programmer
  if (!ispProgrammer->begin()) {
    Serial.println("Failed to initialize ISP programmer");
    return false;
  }

  // Enter programming mode
  if (!ispProgrammer->enterProgrammingMode()) {
    Serial.println("Failed to enter programming mode");
    ispProgrammer->end();
    return false;
  }

  // Show device info
  ispProgrammer->printDeviceInfo();

  // Erase and program
  bool success = false;
  if (ispProgrammer->eraseChip()) {
    success = ispProgrammer->programFlash(hexParser->getFlashBuffer(),
                                          hexParser->getFlashSize());
  }

  // Exit programming mode
  ispProgrammer->exitProgrammingMode();
  ispProgrammer->end();

  if (success) {
    Serial.println("Programming successful!");
  } else {
    Serial.println("Programming failed!");
  }

  return success;
}

void handleCommand(const String& command) {
  if (command.startsWith("program ")) {
    String filename = command.substring(8);
    filename.trim();
    programHexFile(filename);
  } else if (command == "list") {
    listFiles();
  } else if (command == "info") {
    showDeviceInfo();
  } else if (command == "help") {
    showHelp();
  } else if (command.length() > 0) {
    Serial.printf("Unknown command: %s\n", command.c_str());
    Serial.println("Type 'help' for available commands");
  }
}

// ==========================================
// ARDUINO SETUP AND LOOP
// ==========================================
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  while (!Serial.available()) {
    delay(10);
  }

  printWelcome();

  // Initialize filesystem
  if (!initializeFilesystem()) {
    Serial.println("Filesystem initialization failed!");
    return;
  }

  // Initialize WiFi (optional)
  initializeWiFi();

  // Initialize HEX parser
  hexParser = new HexParser(HEX_BUFFER_SIZE);
  if (!hexParser) {
    Serial.println("Failed to create HEX parser");
    return;
  }

  // Initialize ISP programmer
  ispProgrammer =
      new ISPProgrammer(ISP_RESET_PIN, ISP_SCK_PIN, ISP_MOSI_PIN, ISP_MISO_PIN);
  if (!ispProgrammer) {
    Serial.println("Failed to create ISP programmer");
    return;
  }

  // Show available files
  listFiles();

  Serial.println();
  showHelp();
  printConnections();
  Serial.print("Ready> ");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    handleCommand(command);

    Serial.print("Ready> ");
  }

  delay(100);
}

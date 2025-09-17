#include "SerialCLI.h"

#include "ArduboyController.h"
#include "FileSystemManager.h"
#include "OLEDController.h"

SerialCLI::SerialCLI()
    : arduboy(nullptr),
      fileSystem(nullptr),
      oled(nullptr),
      initialized(false) {}

SerialCLI::~SerialCLI() { end(); }

bool SerialCLI::begin(ArduboyController* arduboyController,
                      FileSystemManager* fsManager,
                      OLEDController* oledController) {
  if (initialized) {
    return true;
  }

  arduboy = arduboyController;
  fileSystem = fsManager;
  oled = oledController;

  if (!arduboy || !fileSystem || !oled) {
    Serial.println(
        "Invalid controller, filesystem manager, or OLED controller");
    return false;
  }

  initialized = true;
  printWelcome();
  return true;
}

void SerialCLI::end() {
  initialized = false;
  arduboy = nullptr;
  fileSystem = nullptr;
  oled = nullptr;
}

void SerialCLI::processInput() {
  if (!initialized || !Serial.available()) {
    return;
  }

  String input = Serial.readStringUntil('\n');
  input.trim();

  if (input.length() == 0) {
    printPrompt();
    return;
  }

  // Parse command and arguments
  int spaceIndex = input.indexOf(' ');
  String command = (spaceIndex == -1) ? input : input.substring(0, spaceIndex);
  String args = (spaceIndex == -1) ? "" : input.substring(spaceIndex + 1);

  command.toLowerCase();
  args.trim();

  // Execute commands
  if (command == "help" || command == "?") {
    handleHelp();
  } else if (command == "info") {
    handleInfo();
  } else if (command == "flash") {
    handleFlash(args);
  } else if (command == "check") {
    handleCheck();
  } else if (command == "reset") {
    handleReset();
  } else if (command == "poweron") {
    handlePowerOn();
  } else if (command == "poweroff") {
    handlePowerOff();
  } else if (command == "list" || command == "ls") {
    handleList(args);
  } else if (command == "read" || command == "cat") {
    handleRead(args);
  } else if (command == "write") {
    int spacePos = args.indexOf(' ');
    if (spacePos == -1) {
      printError("Usage: write <filename> <content>");
    } else {
      String filename = args.substring(0, spacePos);
      String content = args.substring(spacePos + 1);
      handleWrite(filename, content);
    }
  } else if (command == "delete" || command == "rm") {
    handleDelete(args);
  } else if (command == "format") {
    handleFormat();
  } else if (command == "oledreset") {
    handleOLEDReset();
  } else if (command == "oledon" || command == "oledenable") {
    handleOLEDEnable();
  } else if (command == "oledoff" || command == "oleddisable") {
    handleOLEDDisable();
  } else if (command == "oledhello") {
    handleOLEDHello();
  } else {
    printError("Unknown command: " + command +
               ". Type 'help' for available commands.");
  }

  printPrompt();
}

void SerialCLI::update() { processInput(); }

// ==========================================
// COMMAND HANDLERS
// ==========================================

void SerialCLI::handleHelp() {
  Serial.println();
  Serial.println("=====================================");
  Serial.println("ARDUBOY FX WIFI - AVAILABLE COMMANDS");
  Serial.println("=====================================");
  Serial.println("System Commands:");
  Serial.println("  help, ?          - Show this help message");
  Serial.println("  info             - Show system information");
  Serial.println();
  Serial.println("Arduboy Commands:");
  Serial.println("  flash <file>     - Flash HEX file to Arduboy");
  Serial.println("  check            - Check Arduboy connection");
  Serial.println("  reset            - Reset Arduboy");
  Serial.println("  poweron          - Turn on Arduboy power");
  Serial.println("  poweroff         - Turn off Arduboy power");
  Serial.println();
  Serial.println("OLED Commands:");
  Serial.println("  oledreset        - Reset OLED display");
  Serial.println("  oledon, oledenable - Enable OLED display");
  Serial.println("  oledoff, oleddisable - Disable OLED display");
  Serial.println("  oledtoggle       - Toggle OLED display state");
  Serial.println("  oledstatus       - Show OLED status");
  Serial.println();
  Serial.println("File System Commands:");
  Serial.println("  list, ls [path]  - List files and directories");
  Serial.println("  read, cat <file> - Read and display file content");
  Serial.println("  write <file> <content> - Write content to file");
  Serial.println("  delete, rm <file> - Delete a file");
  Serial.println("  format           - Format the filesystem");
  Serial.println();
  Serial.println("Examples:");
  Serial.println("  flash firmware.hex");
  Serial.println("  oledreset");
  Serial.println("  oledon");
  Serial.println("  list /");
  Serial.println("  read config.txt");
  Serial.println("  write test.txt Hello World");
  Serial.println("=====================================");
}

void SerialCLI::handleInfo() {
  Serial.println();
  Serial.println("=====================================");
  Serial.println("SYSTEM INFORMATION");
  Serial.println("=====================================");
  Serial.printf("Device: ESP32-C3 SuperMini\n");
  Serial.printf("Firmware: Arduboy FX WiFi Programmer\n");
  Serial.printf("Free Heap: %u bytes\n", ESP.getFreeHeap());
  Serial.printf("Flash Size: %u bytes\n", ESP.getFlashChipSize());
  Serial.printf("CPU Frequency: %u MHz\n", ESP.getCpuFreqMHz());
  Serial.println();

  // Arduboy connection status
  Serial.println("Arduboy Connection:");
  if (arduboy && arduboy->checkConnection()) {
    Serial.println("  Status: Connected");
  } else {
    Serial.println("  Status: Disconnected");
  }
  Serial.println();

  // Filesystem info
  Serial.println("File System:");
  if (fileSystem && fileSystem->isInitialized()) {
    fileSystem->getInfo();
  } else {
    Serial.println("  Status: Not initialized");
  }
}

void SerialCLI::handleFlash(const String& filename) {
  if (filename.length() == 0) {
    printError("Usage: flash <filename>");
    Serial.println("Available HEX files:");
    if (fileSystem) {
      fileSystem->listHexFiles();
    }
    return;
  }

  String filepath = filename.startsWith("/") ? filename : "/" + filename;

  if (!fileSystem || !fileSystem->fileExists(filepath)) {
    printError("File not found: " + filepath);
    return;
  }

  if (!fileSystem->isValidHexFile(filepath)) {
    printError("Invalid HEX file: " + filepath);
    return;
  }

  if (!arduboy || !arduboy->checkConnection()) {
    printError("Arduboy not connected");
    return;
  }

  Serial.println("Starting flash operation...");
  bool success = arduboy->flash(filepath);

  if (success) {
    printSuccess("Flash completed successfully!");
  } else {
    printError("Flash operation failed");
  }
}

void SerialCLI::handleCheck() {
  if (!arduboy) {
    printError("Arduboy controller not initialized");
    return;
  }

  Serial.println("Checking Arduboy connection...");
  bool connected = arduboy->checkConnection();

  if (connected) {
    printSuccess("Arduboy is connected and responding");
  } else {
    printError("Arduboy is not responding or not connected");
  }
}

void SerialCLI::handleReset() {
  if (!arduboy) {
    printError("Arduboy controller not initialized");
    return;
  }

  Serial.println("Resetting Arduboy...");
  arduboy->reset();
  printSuccess("Reset signal sent");
}

void SerialCLI::handlePowerOn() {
  if (!arduboy) {
    printError("Arduboy controller not initialized");
    return;
  }

  Serial.println("Turning on Arduboy power...");
  arduboy->powerOn();
  printSuccess("Power ON signal sent");
}

void SerialCLI::handlePowerOff() {
  if (!arduboy) {
    printError("Arduboy controller not initialized");
    return;
  }

  Serial.println("Turning off Arduboy power...");
  arduboy->powerOff();
  printSuccess("Power OFF signal sent");
}

void SerialCLI::handleList(const String& path) {
  if (!fileSystem) {
    printError("Filesystem not initialized");
    return;
  }

  String listPath = (path.length() == 0) ? "/" : path;
  fileSystem->listDirectory(listPath);
}

void SerialCLI::handleRead(const String& filename) {
  if (filename.length() == 0) {
    printError("Usage: read <filename>");
    return;
  }

  if (!fileSystem) {
    printError("Filesystem not initialized");
    return;
  }

  String filepath = filename.startsWith("/") ? filename : "/" + filename;

  if (!fileSystem->fileExists(filepath)) {
    printError("File not found: " + filepath);
    return;
  }

  String content = fileSystem->readFile(filepath);
  if (content.length() == 0 && fileSystem->getFileSize(filepath) > 0) {
    printError("Failed to read file or file is binary");
    return;
  }

  Serial.println();
  Serial.println("File content:");
  Serial.println("=====================================");
  Serial.println(content);
  Serial.println("=====================================");
}

void SerialCLI::handleWrite(const String& filename, const String& content) {
  if (filename.length() == 0 || content.length() == 0) {
    printError("Usage: write <filename> <content>");
    return;
  }

  if (!fileSystem) {
    printError("Filesystem not initialized");
    return;
  }

  String filepath = filename.startsWith("/") ? filename : "/" + filename;

  if (fileSystem->writeFile(filepath, content)) {
    printSuccess("File written successfully: " + filepath);
  } else {
    printError("Failed to write file: " + filepath);
  }
}

void SerialCLI::handleDelete(const String& filename) {
  if (filename.length() == 0) {
    printError("Usage: delete <filename>");
    return;
  }

  if (!fileSystem) {
    printError("Filesystem not initialized");
    return;
  }

  String filepath = filename.startsWith("/") ? filename : "/" + filename;

  if (!fileSystem->fileExists(filepath)) {
    printError("File not found: " + filepath);
    return;
  }

  if (fileSystem->deleteFile(filepath)) {
    printSuccess("File deleted successfully: " + filepath);
  } else {
    printError("Failed to delete file: " + filepath);
  }
}

void SerialCLI::handleFormat() {
  Serial.println("WARNING: This will erase all files!");
  Serial.println("Type 'YES' to confirm format operation:");

  // Wait for confirmation
  unsigned long timeout = millis() + 10000;  // 10 second timeout
  String confirmation = "";

  while (millis() < timeout && confirmation.length() == 0) {
    if (Serial.available()) {
      confirmation = Serial.readStringUntil('\n');
      confirmation.trim();
      break;
    }
    delay(100);
  }

  if (confirmation == "YES") {
    if (fileSystem && fileSystem->format()) {
      printSuccess("Filesystem formatted successfully");
    } else {
      printError("Format operation failed");
    }
  } else {
    Serial.println("Format operation cancelled");
  }
}

// ==========================================
// OLED COMMAND HANDLERS
// ==========================================

void SerialCLI::handleOLEDReset() {
  if (!oled) {
    printError("OLED controller not initialized");
    return;
  }

  if (oled->reset()) {
    printSuccess("OLED reset completed");
  } else {
    printError("OLED reset failed");
  }
}

void SerialCLI::handleOLEDEnable() {
  if (!oled) {
    printError("OLED controller not initialized");
    return;
  }

  if (oled->enable()) {
    printSuccess("OLED enabled");
  } else {
    printError("Failed to enable OLED");
  }
}

void SerialCLI::handleOLEDDisable() {
  if (!oled) {
    printError("OLED controller not initialized");
    return;
  }

  if (oled->disable()) {
    printSuccess("OLED disabled");
  } else {
    printError("Failed to disable OLED");
  }
}

void SerialCLI::handleOLEDHello() {
  if (!oled) {
    printError("OLED controller not initialized");
    return;
  }

  oled->helloWorld();
}

// ==========================================
// UTILITY FUNCTIONS
// ==========================================

void SerialCLI::printWelcome() {
  Serial.println();
  Serial.println("=====================================");
  Serial.println("    ARDUBOY FX WIFI PROGRAMMER");
  Serial.println("=====================================");
  Serial.println("ESP32-C3 Based ISP Programmer");
  Serial.println("Type 'help' for available commands");
  Serial.println("=====================================");
  printPrompt();
}

void SerialCLI::printPrompt() { Serial.print("ArduFX> "); }

void SerialCLI::printError(const String& message) {
  Serial.print("[ERROR] ");
  Serial.println(message);
}

void SerialCLI::printSuccess(const String& message) {
  Serial.print("[SUCCESS] ");
  Serial.println(message);
}

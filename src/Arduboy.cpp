#include "Arduboy.h"

#include "config.h"

Arduboy::Arduboy()
    : hexParser(nullptr), ispProgrammer(nullptr), initialized(false) {}

Arduboy::~Arduboy() { end(); }

bool Arduboy::begin() {
  if (initialized) {
    return true;
  }

  // Initialize HEX parser
  hexParser = new HexParser(HEX_BUFFER_SIZE);
  if (!hexParser) {
    Serial.println("Failed to create HEX parser");
    return false;
  }

  // Initialize ISP programmer
  ispProgrammer =
      new ISPProgrammer(ISP_RESET_PIN, ISP_SCK_PIN, ISP_MOSI_PIN, ISP_MISO_PIN);
  if (!ispProgrammer) {
    Serial.println("Failed to create ISP programmer");
    delete hexParser;
    hexParser = nullptr;
    return false;
  }

  initialized = true;
  return true;
}

void Arduboy::end() {
  if (hexParser) {
    delete hexParser;
    hexParser = nullptr;
  }
  if (ispProgrammer) {
    delete ispProgrammer;
    ispProgrammer = nullptr;
  }
  initialized = false;
}

bool Arduboy::checkConnection() {
  if (!initialized || !ispProgrammer) {
    Serial.println("Arduboy not initialized");
    return false;
  }

  if (!ispProgrammer->begin()) {
    Serial.println("Failed to initialize ISP programmer");
    return false;
  }

  bool connected = ispProgrammer->enterProgrammingMode();
  if (connected) {
    Serial.println("Arduboy connected successfully");
    ispProgrammer->exitProgrammingMode();
  } else {
    Serial.println("Failed to connect to Arduboy");
  }

  ispProgrammer->end();
  return connected;
}

bool Arduboy::flash(const String& filename) {
  if (!initialized || !hexParser || !ispProgrammer) {
    Serial.println("Arduboy not initialized");
    return false;
  }

  String fullPath = filename;
  if (!fullPath.startsWith("/")) {
    fullPath = "/" + fullPath;
  }

  Serial.printf("Flashing Arduboy with: %s\n", fullPath.c_str());

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
    Serial.println("Flashing successful!");
  } else {
    Serial.println("Flashing failed!");
  }

  return success;
}

bool Arduboy::reset() {
  if (!initialized || !ispProgrammer) {
    Serial.println("Arduboy not initialized");
    return false;
  }

  if (!ispProgrammer->begin()) {
    Serial.println("Failed to initialize ISP programmer");
    return false;
  }

  // Trigger reset by toggling reset pin
  Serial.println("Resetting Arduboy...");

  // The reset is handled internally by the ISP programmer
  // We'll exit programming mode which releases reset
  ispProgrammer->exitProgrammingMode();

  ispProgrammer->end();
  Serial.println("Reset complete");
  return true;
}

void Arduboy::printDeviceInfo() {
  if (!initialized || !ispProgrammer) {
    Serial.println("Arduboy not initialized");
    return;
  }

  if (!ispProgrammer->begin()) {
    Serial.println("Failed to initialize ISP programmer");
    return;
  }

  if (ispProgrammer->enterProgrammingMode()) {
    ispProgrammer->printDeviceInfo();
    ispProgrammer->printFuses();
    ispProgrammer->exitProgrammingMode();
  } else {
    Serial.println("Failed to enter programming mode");
  }

  ispProgrammer->end();
}

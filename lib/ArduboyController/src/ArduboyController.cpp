#include "ArduboyController.h"


ArduboyController::ArduboyController() {
  hexParser = nullptr;
  ispProgrammer = nullptr;
  initialized = false;
}

ArduboyController::~ArduboyController() { end(); }

bool ArduboyController::begin(uint8_t pinReset, uint32_t hexBufferSize) {
  if (initialized) {
    return true;
  }

  this->pinReset = pinReset;

  pinMode(this->pinReset, OUTPUT);

  // Initialize HEX parser
  hexParser = new HexParser(hexBufferSize);
  if (!hexParser) {
    Logger::error("Failed to create HEX parser");
    return false;
  }

  // Initialize ISP programmer (using default hardware SPI pins)
  ispProgrammer = new ISPProgrammer(this->pinReset);
  if (!ispProgrammer) {
    Logger::error("Failed to create ISP programmer");
    delete hexParser;
    hexParser = nullptr;
    return false;
  }

  initialized = true;
  return true;
}

void ArduboyController::end() {
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

bool ArduboyController::checkConnection() {
  if (!initialized || !ispProgrammer) {
    Logger::error("ArduboyController not initialized");
    return false;
  }

  if (!ispProgrammer->begin()) {
    Logger::error("Failed to initialize ISP programmer");
    return false;
  }

  bool connected = ispProgrammer->enterProgrammingMode();
  if (connected) {
    Logger::info("Arduboy connected successfully");
    ispProgrammer->exitProgrammingMode();
  } else {
    Logger::error("Failed to connect to Arduboy device");
  }

  ispProgrammer->end();
  return connected;
}

bool ArduboyController::flash(File& file) {
  if (!initialized || !hexParser || !ispProgrammer) {
    Logger::error("ArduboyController not initialized");
    return false;
  }

  Logger::info("Flashing Arduboy with: %s\n", file.name());

  if (!file) {
    Logger::error("Failed to open HEX file");
    return false;
  }

  // Parse HEX file
  if (!hexParser->parseFile(file)) {
    Logger::error("Failed to parse HEX file");
    return false;
  }

  // Initialize ISP programmer
  if (!ispProgrammer->begin()) {
    Logger::error("Failed to initialize ISP programmer");
    return false;
  }

  // Enter programming mode
  if (!ispProgrammer->enterProgrammingMode()) {
    Logger::error("Failed to enter programming mode");
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
    Logger::info("Flashing successful!");
  } else {
    Logger::error("Flashing failed!");
  }

  return success;
}

bool ArduboyController::reset() {
  // Trigger reset by toggling reset pin
  Logger::info("Resetting Arduboy...");

  this->powerOff();
  delay(100);
  this->powerOn();
  delay(100);

  Logger::info("Reset complete");
  return true;
}

void ArduboyController::printDeviceInfo() {
  if (!initialized || !ispProgrammer) {
    Logger::error("ArduboyController not initialized");
    return;
  }

  if (!ispProgrammer->begin()) {
    Logger::error("Failed to initialize ISP programmer");
    return;
  }

  if (ispProgrammer->enterProgrammingMode()) {
    ispProgrammer->printDeviceInfo();
    ispProgrammer->printFuses();
    ispProgrammer->exitProgrammingMode();
  } else {
    Logger::error("Failed to enter programming mode");
  }

  ispProgrammer->end();
}

bool ArduboyController::powerOn() {
  Logger::info("Powering on Arduboy...");

  digitalWrite(this->pinReset, HIGH);

  // Give some time for power to stabilize
  delay(100);

  Logger::info("Arduboy powered on");
  return true;
}

bool ArduboyController::powerOff() {
  Logger::info("Powering off Arduboy...");

  digitalWrite(this->pinReset, LOW);

  Logger::info("Arduboy powered off");
  return true;
}

#include "ArduboyController.h"

static bool oledSSD1309Patch(uint8_t* buf, uint32_t buf_size, uint32_t flash_size, void* ctx) {
  if (!buf) return false;
  static const uint8_t default_lcdBootProgram[] = {
    0xD5, 0xF0, 0x8D, 0x14, 0xA1, 0xC8, 0x81, 0xCF, 0xD9, 0xF1, 0xAF, 0x20, 0x00
};
  static constexpr size_t default_lcdBootProgram_len =
      sizeof(default_lcdBootProgram) / sizeof(default_lcdBootProgram[0]);
  for (uint32_t i = 0; i + default_lcdBootProgram_len <= buf_size; ++i) {
    bool ok = true;
    for (size_t j = 0; j < default_lcdBootProgram_len; ++j) {
      if (buf[i + j] != default_lcdBootProgram[j]) { ok = false; break; }
    }
    if (ok) {
      if ((size_t)i + 3 < buf_size) {
        buf[i + 2] = 0xE3;
        buf[i + 3] = 0xE3;
        return true;
      }
      return false;
    }
  }
  return false;
}

ArduboyController::ArduboyController() {}

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

  // Apply OLED patch if needed
  if (!hexParser->modifyBuffer(oledSSD1309Patch, nullptr)) {
    Logger::error("Failed to apply OLED patch to HEX file");
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

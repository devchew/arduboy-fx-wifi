#include "ArduboyController.h"

#include "config.h"

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

ArduboyController::ArduboyController()
    : hexParser(nullptr), ispProgrammer(nullptr), initialized(false) {}

ArduboyController::~ArduboyController() { end(); }

bool ArduboyController::begin() {
  if (initialized) {
    return true;
  }

  pinMode(ISP_RESET_PIN, OUTPUT);

  // Initialize HEX parser
  hexParser = new HexParser(HEX_BUFFER_SIZE);
  if (!hexParser) {
    Serial.println("Failed to create HEX parser");
    return false;
  }

  // Initialize ISP programmer (using default hardware SPI pins)
  ispProgrammer = new ISPProgrammer(ISP_RESET_PIN);
  if (!ispProgrammer) {
    Serial.println("Failed to create ISP programmer");
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
    Serial.println("ArduboyController not initialized");
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
    Serial.println("Failed to connect to Arduboy device");
  }

  ispProgrammer->end();
  return connected;
}

bool ArduboyController::flash(const String& filename) {
  if (!initialized || !hexParser || !ispProgrammer) {
    Serial.println("ArduboyController not initialized");
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

bool ArduboyController::reset() {

  // Trigger reset by toggling reset pin
  Serial.println("Resetting Arduboy...");

  this->powerOff();
  delay(100);
  this->powerOn();
  delay(100);

  Serial.println("Reset complete");
  return true;
}

void ArduboyController::printDeviceInfo() {
  if (!initialized || !ispProgrammer) {
    Serial.println("ArduboyController not initialized");
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

bool ArduboyController::powerOn() {
  Serial.println("Powering on Arduboy...");

  digitalWrite(ISP_RESET_PIN, HIGH);

  // Give some time for power to stabilize
  delay(100);
  
  Serial.println("Arduboy powered on");
  return true;
}

bool ArduboyController::powerOff() {
  Serial.println("Powering off Arduboy...");

  digitalWrite(ISP_RESET_PIN, LOW);

  Serial.println("Arduboy powered off");
  return true;
}

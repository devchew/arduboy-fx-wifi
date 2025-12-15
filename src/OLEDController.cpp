#include "OLEDController.h"

OLEDController::OLEDController() : u8g2(U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI(U8G2_R0,
    /* cs=*/U8X8_PIN_NONE,
    /* dc=*/OLED_DC_PIN,
    /* reset=*/OLED_RESET_PIN)), initialized(false), isMaster(false) {}

OLEDController::~OLEDController() { end(); }

bool OLEDController::begin() {
  if (initialized) {
    return true;
  }

  // Setup pins - initialize as OUTPUT but don't drive them yet
  pinMode(OLED_CS_PIN, OUTPUT);
  pinMode(OLED_DC_PIN, OUTPUT);
  pinMode(OLED_RESET_PIN, OUTPUT);

  enable();
  master();

  // Don't initialize display yet - wait for master() call
  initialized = true;
  Logger::info("OLED Controller initialized successfully");
  return true;
}

void OLEDController::end() {
  if (initialized) {
    // Set pins to safe state
    u8g2.setPowerSave(1);  // Turn off display

    initialized = false;
    isMaster = false;
  }
}

// ==========================================
// BASIC CONTROL FUNCTIONS
// ==========================================

bool OLEDController::reset() {
  if (!initialized) {
    Logger::error("OLED Controller not initialized");
    return false;
  }

  this->disable();
  delay(10);
  this->enable();
  delay(10);

  Logger::info("OLED reset completed");
  return true;
}

bool OLEDController::slave() {
  if (!initialized) {
    Logger::error("OLED Controller not initialized");
    return false;
  }

  if (!isMaster) {
    Logger::error("OLED already in SLAVE mode");
    return true;
  }

  isMaster = false;

  u8g2.clearDisplay();
  u8g2.setPowerSave(1);  // Turn off display

  // Release control pins to allow AVR to control OLED
  pinMode(OLED_DC_PIN, INPUT);     // High-impedance DC
  pinMode(OLED_RESET_PIN, INPUT);  // High-impedance RESET

  Logger::info("OLED set to SLAVE mode - control pins released");
  return true;
}

bool OLEDController::master() {
  if (!initialized) {
    Logger::error("OLED Controller not initialized");
    return false;
  }

  // Reclaim control pins
  pinMode(OLED_DC_PIN, OUTPUT);
  pinMode(OLED_RESET_PIN, OUTPUT);

  // Initialize the display
  u8g2.begin();
  u8g2.setBusClock(1000000);  // 1MHz
  u8g2.setPowerSave(0);
  u8g2.setFlipMode(0);
  u8g2.setContrast(128);
  u8g2.clearDisplay();
  u8g2.setPowerSave(0);  // Turn on display

  isMaster = true;
  Logger::info("OLED set to MASTER mode - control pins reclaimed");
  return true;
}

bool OLEDController::enable() {
  if (!initialized) {
    Logger::error("OLED Controller not initialized");
    return false;
  }

  digitalWrite(OLED_CS_PIN, LOW);  // Select OLED
  Logger::info("OLED enabled");
  return true;
}

bool OLEDController::disable() {
  if (!initialized) {
    Logger::error("OLED Controller not initialized");
    return false;
  }

  digitalWrite(OLED_CS_PIN, HIGH);  // Deselect OLED
  Logger::info("OLED disabled");
  return true;
}

void OLEDController::clear() {
  if (!initialized || !isMaster) {
    return;
  }

  u8g2.clearDisplay();
}
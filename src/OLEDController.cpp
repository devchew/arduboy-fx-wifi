#include "OLEDController.h"

#include "config.h"

OLEDController::OLEDController()
    : initialized(false),
      isMaster(true),
      u8x8(U8X8_SSD1309_128X64_NONAME0_4W_HW_SPI(/* cs=*/U8X8_PIN_NONE,
                                                 /* dc=*/OLED_DC_PIN,
                                                 /* reset=*/OLED_RESET_PIN)) {}

OLEDController::~OLEDController() { end(); }

bool OLEDController::begin() {
  if (initialized) {
    return true;
  }

  // Setup pins
  pinMode(OLED_CS_PIN, OUTPUT);

  // Set initial pin states
  digitalWrite(OLED_CS_PIN, LOW);  // CS inactive
  // Initialize the displayo
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFlipMode(0);
  u8x8.setContrast(128);
  u8x8.clearDisplay();

  initialized = true;
  Serial.println("OLED Controller initialized successfully");
  return true;
}

void OLEDController::end() {
  if (initialized) {
    // Set pins to safe state
    u8x8.setPowerSave(1);  // Turn off display

    initialized = false;
    isMaster = false;
  }
}

// ==========================================
// BASIC CONTROL FUNCTIONS
// ==========================================

bool OLEDController::reset() {
  if (!initialized) {
    Serial.println("OLED Controller not initialized");
    return false;
  }

  this->disable();
  delay(10);
  this->enable();
  delay(10);

  Serial.println("OLED reset completed");
  return true;
}

bool OLEDController::slave() {
  if (!initialized) {
    Serial.println("OLED Controller not initialized");
    return false;
  }

  if (!isMaster) {
    Serial.println("OLED already in SLAVE mode");
    return true;
  }

  isMaster = false;

  u8x8.clearDisplay();
  u8x8.setPowerSave(1);  // Turn off display
  currentScreen = 0;
  Serial.println("OLED set to SLAVE mode");
  return true;
}

bool OLEDController::master() {
  if (!initialized) {
    Serial.println("OLED Controller not initialized");
    return false;
  }

  if (isMaster) {
    Serial.println("OLED already in MASTER mode");
    return true;
  }

  isMaster = true;

  u8x8.clearDisplay();
  u8x8.setPowerSave(0);  // Turn on display
  currentScreen = 0;
  Serial.println("OLED set to MASTER mode");
  return true;
}

bool OLEDController::enable() {
  if (!initialized) {
    Serial.println("OLED Controller not initialized");
    return false;
  }

  if (!isMaster) {
    Serial.println("OLED must be in MASTER mode to enable");
    return false;
  }

  digitalWrite(OLED_CS_PIN, LOW);  // Select OLED
  Serial.println("OLED enabled");
  return true;
}

bool OLEDController::disable() {
  if (!initialized) {
    Serial.println("OLED Controller not initialized");
    return false;
  }

  if (!isMaster) {
    Serial.println("OLED must be in MASTER mode to enable");
    return false;
  }

  digitalWrite(OLED_CS_PIN, HIGH);  // Deselect OLED
  Serial.println("OLED disabled");
  return true;
}

void OLEDController::helloWorld() {
  if (!initialized) {
    Serial.println("OLED Controller not initialized");
    return;
  }

  if (!isMaster) {
    Serial.println("OLED must be in MASTER mode to display");
    return;
  }

  currentScreen = 1;
}

void OLEDController::clear() {
  if (!initialized || !isMaster) {
    return;
  }

  u8x8.clearDisplay();
  currentScreen = 0;
}

void OLEDController::loop() {
  if (!initialized || !isMaster) {
    return;
  }

  if (currentScreen == 1) {
    u8x8.clearDisplay();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(0, 1, "Hello esp!");
    u8x8.drawString(0, 3, "Arduboy FX");
  }
}
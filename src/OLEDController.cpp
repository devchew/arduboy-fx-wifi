#include "OLEDController.h"

#include "config.h"

OLEDController::OLEDController()
    : initialized(false),
      isMaster(false),
      u8x8(U8X8_SSD1309_128X64_NONAME0_4W_HW_SPI(/* cs=*/U8X8_PIN_NONE,
                                                 /* dc=*/OLED_DC_PIN,
                                                 /* reset=*/OLED_RESET_PIN)) {}

OLEDController::~OLEDController() { end(); }

bool OLEDController::begin() {
  if (initialized) {
    return true;
  }

  // Setup pins - initialize as OUTPUT but don't drive them yet
  pinMode(OLED_CS_PIN, OUTPUT);
  pinMode(OLED_DC_PIN, OUTPUT);
  pinMode(OLED_RESET_PIN, OUTPUT);

  // Set safe initial states
  digitalWrite(OLED_CS_PIN, HIGH);  // Deselect OLED

  // Don't initialize display yet - wait for master() call
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

  // Release control pins to allow AVR to control OLED
  pinMode(OLED_DC_PIN, INPUT);     // High-impedance DC
  pinMode(OLED_RESET_PIN, INPUT);  // High-impedance RESET

  Serial.println("OLED set to SLAVE mode - control pins released");
  return true;
}

bool OLEDController::master() {
  if (!initialized) {
    Serial.println("OLED Controller not initialized");
    return false;
  }

  // Reclaim control pins
  pinMode(OLED_DC_PIN, OUTPUT);
  pinMode(OLED_RESET_PIN, OUTPUT);

  // Set safe initial states
  digitalWrite(OLED_CS_PIN, HIGH);  // Deselect OLED initially

  // Initialize the display
  u8x8.begin();
  u8x8.setBusClock(1000000);  // 1MHz
  u8x8.setPowerSave(0);
  u8x8.setFlipMode(0);
  u8x8.setContrast(128);
  u8x8.clearDisplay();
  u8x8.setPowerSave(0);  // Turn on display

  isMaster = true;
  Serial.println("OLED set to MASTER mode - control pins reclaimed");
  return true;
}

bool OLEDController::enable() {
  if (!initialized) {
    Serial.println("OLED Controller not initialized");
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

  u8x8.clearDisplay();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 1, "Hello esp!");
  u8x8.drawString(0, 3, "Arduboy FX");
}

void OLEDController::clear() {
  if (!initialized || !isMaster) {
    return;
  }

  u8x8.clearDisplay();
}

void OLEDController::loop() {
  if (!initialized || !isMaster) {
    return;
  }
  // if (millis() % 1000 == 0) {
  //   helloWorld();
  // }
}
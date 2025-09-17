#include "OLEDController.h"

#include "config.h"

OLEDController::OLEDController()
    : initialized(false),
      enabled(false),
      resetState(false),
      enableState(false),
      u8x8(U8X8_SSD1306_128X64_NONAME_4W_HW_SPI(/* cs=*/OLED_CS_PIN,
                                                /* dc=*/OLED_DC_PIN,
                                                /* reset=*/OLED_RESET_PIN)) {}

OLEDController::~OLEDController() { end(); }

bool OLEDController::begin() {
  if (initialized) {
    return true;
  }
  // Initialize the display
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFlipMode(1);
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
    enabled = false;
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

  // Perform hardware reset sequence
  u8x8.setPowerSave(1);  // Assert reset
  delay(10);             // Hold reset for 10ms
  u8x8.setPowerSave(0);  // Release reset
  delay(10);             // Allow OLED to stabilize

  resetState = false;  // Reset completed
  Serial.println("OLED reset completed");
  return true;
}

bool OLEDController::enable() {
  if (!initialized) {
    Serial.println("OLED Controller not initialized");
    return false;
  }

  // Enable OLED by setting CS low (active low)
  u8x8.setPowerSave(0);
  enableState = true;

  Serial.println("OLED enabled");
  return true;
}

bool OLEDController::disable() {
  if (!initialized) {
    Serial.println("OLED Controller not initialized");
    return false;
  }

  // Disable OLED by setting CS high (inactive)
  u8x8.setPowerSave(1);
  enableState = false;

  Serial.println("OLED disabled");
  return true;
}

void OLEDController::helloWorld() {
  if (!initialized) {
    Serial.println("OLED Controller not initialized");
    return;
  }

  u8x8.clearDisplay();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 1, "Hello esp!");
}
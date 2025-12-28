#include <Arduino.h>
#include <WiFi.h>

#include "FxManager.h"
#include "SerialCLI.h"
#include "config.h"
#include<MacroLogger.h>

// ==========================================
// GLOBAL OBJECTS
// ==========================================

SerialCLI* cli = nullptr;
FxManager* fxManager = nullptr;

// ==========================================
// UTILITY FUNCTIONS
// ==========================================

void initializeWiFi() {
  if (WIFI_ENABLED) {
    Logger::info("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Logger::info(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Logger::info();
      Logger::info("WiFi connected");
      Logger::info("IP address: ");
      // Logger::info(WiFi.localIP().toString());
    } else {
      Logger::info();
      Logger::info("WiFi connection failed, continuing without WiFi");
    }
  }
}

// ==========================================
// ARDUINO SETUP AND LOOP
// ==========================================

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Logger::set_level(Logger::Level::WARNING);

  Logger::info("Starting Arduboy FX WiFi Programmer...");

  // Initialize WiFi (optional)
  // initializeWiFi();

  fxManager = new FxManager();
  if (!fxManager || !fxManager->begin()) {
    Logger::error("Failed to initialize FxManager!");
    return;
  }

  Logger::info("FxManager initialized successfully");

  // Initialize SerialCLI
  cli = new SerialCLI(fxManager);
  if (!cli) {
    Logger::error("Failed to initialize Serial CLI!");
    return;
  }

  Logger::info("Serial CLI initialized successfully");

}

void loop() {
  if (cli) {
    cli->update();
  }
  if (fxManager) {
    fxManager->update();
  }
  yield();
}

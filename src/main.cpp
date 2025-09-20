#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include "FxManager.h"
#include "SerialCLI.h"
#include "config.h"

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
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println();
      Serial.println("WiFi connection failed, continuing without WiFi");
    }
  }
}

// ==========================================
// ARDUINO SETUP AND LOOP
// ==========================================
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  // Initialize WiFi (optional)
  initializeWiFi();

  fxManager = new FxManager();
  if (!fxManager || !fxManager->begin()) {
    Serial.println("Failed to initialize FxManager!");
    return;
  }

  // Initialize SerialCLI
  cli = new SerialCLI(fxManager);
  if (!cli) {
    Serial.println("Failed to initialize Serial CLI!");
    return;
  }
}

void loop() {
  if (cli) {
    cli->update();
  }
  delay(10);
}

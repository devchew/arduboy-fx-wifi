#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include "ArduboyController.h"
#include "FileSystemManager.h"
#include "SerialCLI.h"
#include "config.h"

// ==========================================
// GLOBAL OBJECTS
// ==========================================
ArduboyController* arduboy = nullptr;
FileSystemManager* fileSystem = nullptr;
SerialCLI* cli = nullptr;

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

  Serial.println();
  Serial.println("Initializing Arduboy FX WiFi Programmer...");

  // Initialize FileSystem
  fileSystem = new FileSystemManager();
  if (!fileSystem || !fileSystem->begin()) {
    Serial.println("Failed to initialize filesystem!");
    return;
  }

  // Initialize WiFi (optional)
  initializeWiFi();

  // Initialize ArduboyController
  arduboy = new ArduboyController();
  if (!arduboy || !arduboy->begin()) {
    Serial.println("Failed to initialize ArduboyController!");
    return;
  }

  // Initialize SerialCLI
  cli = new SerialCLI();
  if (!cli || !cli->begin(arduboy, fileSystem)) {
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

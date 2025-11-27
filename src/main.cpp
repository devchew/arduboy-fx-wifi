#include <Arduino.h>
#include <WiFi.h>

#include "FxManager.h"
#include "SerialCLI.h"
#include "config.h"

#include "SD.h"
#include <SPI.h>

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

  Serial.println();
  Serial.println("Starting Arduboy FX WiFi Programmer...");

  // Initialize WiFi (optional)
  // initializeWiFi();

  fxManager = new FxManager();
  if (!fxManager || !fxManager->begin()) {
    Serial.println("Failed to initialize FxManager!");
    return;
  }

  Serial.println("FxManager initialized successfully");

  // Initialize SerialCLI
  cli = new SerialCLI(fxManager);
  if (!cli) {
    Serial.println("Failed to initialize Serial CLI!");
    return;
  }

  Serial.println("Serial CLI initialized successfully");

  //SDCARD
  pinMode(SD_CS_PIN, OUTPUT); // SS

  SPIClass sdSPI(2);
  sdSPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  if (!SD.begin(SD_CS_PIN, sdSPI)) {
    Serial.println("Nie udalo sie zainicjowac karty SD");
  }

  uint8_t cardType = SD.cardType();

  switch (cardType) {
    case CARD_NONE:
      Serial.println("Brak karty SD");
      break;
    case CARD_MMC:
      Serial.println("Karta MMC");
      break;
    case CARD_SD:
      Serial.println("Karta SDSC");
      break;
    case CARD_SDHC:
      Serial.println("Karta SDHC");
      break;
    default:
      Serial.println("Nieznany typ karty");
  }
  // delay(100);

  // fxManager->setMode(FxMode::MASTER);

  Serial.println("Setup complete. Enter commands:");
}

void loop() {
  if (cli) {
    cli->update();
  }
  if (fxManager) {
    fxManager->update();
  }
  if (millis() % 1000 == 0) {
    Serial.println();
    Serial.print("SD Card Type: ");
    Serial.println(SD.cardType());
  }
  yield();
}

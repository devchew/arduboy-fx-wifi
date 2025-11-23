#include "FxManager.h"

FxManager::FxManager() {
  arduboy = nullptr;
  fileSystem = nullptr;
  oled = nullptr;
  initialized = false;
}

FxManager::~FxManager() {
  delete arduboy;
  delete fileSystem;
  delete oled;
}

bool FxManager::begin() {
  // Initialize FileSystem
  fileSystem = new FileSystemManager();
  if (!fileSystem || !fileSystem->begin()) {
    Serial.println("Failed to initialize filesystem!");
    return false;
  }

  // todo: SPI line sharing with oled fight each other

  // Initialize ArduboyController
  arduboy = new ArduboyController();
  if (!arduboy || !arduboy->begin()) {
    Serial.println("Failed to initialize ArduboyController!");
    return false;
  }

  // Initialize OLEDController
  oled = new OLEDController();
  if (!oled || !oled->begin()) {
    Serial.println("Failed to initialize OLEDController!");
    return false;
  }

  initialized = true;

  setMode(FxMode::MASTER);
  return true;
}

void FxManager::update() {
  if (oled) oled->loop();
}

void FxManager::setMode(FxMode mode) {
  if (!initialized) {
    Serial.println("[error] FxManager not initialized");
    return;
  }

  currentMode = mode;

  switch (mode) {
    case FxMode::GAME:
      // First put OLED into slave mode (releases control pins)
      oled->slave();
      oled->enable();
      delay(50);  // Give time for OLED to enter sleep and release pins

      // Then tri-state SPI pins to let AVR take control
      this->triStateSPIPins();
      delay(20);  // Give time for pin state changes

      // Finally power on AVR
      arduboy->powerOn();

      Serial.println("Switched to GAME mode");
      break;
    case FxMode::MASTER:
      // First power off AVR to prevent conflicts
      arduboy->powerOff();
      delay(50);  // Give time for AVR to power down completely

      // Reactivate SPI for ESP control
      this->triStateSPIPins();
      this->activateSPIPins();
      delay(20);  // Give time for SPI reactivation

      oled->master();
      // Enable and configure OLED in master mode
      oled->enable();
      delay(50);  // Give extra time for display initialization

      oled->helloWorld();

      Serial.println("Switched to MASTER mode");
      break;
    case FxMode::PROGRAMMING:
      oled->clear();
      oled->slave();
      oled->disable();
      delay(10);
      this->triStateSPIPins();
      this->activateSPIPins();

      arduboy->powerOn();
      delay(50);  // allow bus and target to settle before probing
      Serial.println("Switched to PROGRAMMING mode");
      break;
  }
}

void FxManager::flashGame(const String& filename) {
  if (!initialized) {
    Serial.println("[error] FxManager not initialized");
    return;
  }

  setMode(FxMode::PROGRAMMING);

  if (filename.length() == 0) {
    Serial.println("[error] Usage: flash <filename>");
    Serial.println("Available HEX files:");
    if (fileSystem) {
      fileSystem->listHexFiles();
    }
    return;
  }

  String filepath = filename.startsWith("/") ? filename : "/" + filename;

  if (!fileSystem || !fileSystem->fileExists(filepath)) {
    Serial.println("[error] File not found: " + filepath);
    return;
  }

  if (!fileSystem->isValidHexFile(filepath)) {
    Serial.println("[error] Invalid HEX file: " + filepath);
    return;
  }

  if (!arduboy || !arduboy->checkConnection()) {
    Serial.println("[error] Arduboy not connected");
    return;
  }

  Serial.println("Starting flash operation...");
  bool success = arduboy->flash(filepath);

  if (success) {
    Serial.println("[success] Flash completed successfully!");
  } else {
    Serial.println("[error] Flash operation failed");
  }

  setMode(FxMode::GAME);
}

void FxManager::listGames() {
  if (!initialized) {
    Serial.println("[error] FxManager not initialized");
    return;
  }

  if (fileSystem) {
    fileSystem->listHexFiles();
  } else {
    Serial.println("[error] Filesystem not initialized");
  }
}

void FxManager::reset() {
  if (!initialized) {
    Serial.println("[error] FxManager not initialized");
    return;
  }

  if (this->currentMode == FxMode::MASTER) {
    return;
  }

  if (arduboy->reset()) {
    Serial.println("[success] Arduboy reset successfully");
  } else {
    Serial.println("[error] Failed to reset Arduboy");
  }
}

void FxManager::printInfo() {
  if (!initialized) {
    Serial.println("[error] FxManager not initialized");
    return;
  }

  this->setMode(FxMode::PROGRAMMING);

  delay(50);

  if (arduboy) {
    arduboy->printDeviceInfo();
  } else {
    Serial.println("[error] ArduboyController not initialized");
  }

  if (oled) {
    oled->isInitialized() ? Serial.println("OLEDController initialized")
                          : Serial.println("OLEDController not initialized");
    oled->isMasterMode() ? Serial.println("OLEDController in MASTER mode")
                         : Serial.println("OLEDController in SLAVE mode");
  } else {
    Serial.println("[error] OLEDController not initialized");
  }

  this->setMode(FxMode::MASTER);
}

void FxManager::triStateSPIPins() {
  // Set SPI pins to INPUT (high-impedance) to avoid interference
  SPI.end();  // End SPI if active
  pinMode(SCK, INPUT);
  pinMode(MOSI, INPUT);
  pinMode(MISO, INPUT);  // keep line stable
}

void FxManager::activateSPIPins() {
  // Set SPI pins to OUTPUT for active communication
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);  // host reads MISO
  SPI.begin();
}
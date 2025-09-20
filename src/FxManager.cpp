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
      this->triStateSPIPins();
      oled->enable();
      oled->slave();
      arduboy->powerOn();

      Serial.println("Switched to GAME mode");
      break;
    case FxMode::MASTER:
      arduboy->powerOff();
      this->activateSPIPins();
      oled->enable();
      oled->master();
      oled->helloWorld();
      Serial.println("Switched to MASTER mode");
      break;
    case FxMode::PROGRAMMING:
      oled->clear();
      oled->disable();
      oled->slave();
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

  setMode(FxMode::MASTER);
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
  pinMode(MISO, INPUT_PULLUP);  // keep line stable
  // ensure hardware SS is tri-stated / not driving bus
  pinMode(SS, INPUT);
}
// ...existing code...
void FxManager::activateSPIPins() {
  // Set SPI pins to OUTPUT for active communication
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);  // host reads MISO
  // configure SS for AVR slave select and set idle HIGH
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  SPI.begin();
}
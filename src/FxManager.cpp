#include "FxManager.h"
#include "UI.h"

FxManager::FxManager() {
  arduboy = nullptr;
  fileSystem = nullptr;
  oled = nullptr;
  ui = nullptr;
  hid = nullptr;
  initialized = false;
  currentMode = FxMode::MASTER;
}

FxManager::~FxManager() {
  delete arduboy;
  delete fileSystem;
  delete oled;
}

bool FxManager::begin() {
  // Initialize FileSystem
  fileSystem = new FileSystemManager();
  if (!fileSystem->begin()) {
    Serial.println("Failed to initialize filesystem!");
    return false;
  }

  // Initialize ArduboyController
  arduboy = new ArduboyController();
  if (!arduboy->begin(ISP_RESET_PIN, HEX_BUFFER_SIZE)) {
    Serial.println("Failed to initialize ArduboyController!");
    return false;
  }

  hid = new HID();
  if (!hid->begin()) {
    Serial.println("Failed to initialize HID!");
    return false;
  }

  // Initialize OLEDController
  oled = new OLEDController();
  if (!oled->begin()) {
    Serial.println("Failed to initialize OLEDController!");
    return false;
  }

  ui = new UI();
  if (!ui->begin(oled->u8g2, *hid, *this)) {
    Serial.println("Failed to initialize UI!");
    return false;
  }

  initialized = true;

  setMode(FxMode::MASTER);
  return true;
}

void FxManager::update() {
  if (!initialized) {
    return;
  }

  if (currentMode == FxMode::MASTER) {
    hid->update();
    ui->update();
    if (hid->pressed(Buttons::START)) {
      setMode(FxMode::GAME);
    }
  }

  if (currentMode == FxMode::GAME) {
    hid->update();
    if (hid->pressed(Buttons::SELECT)) {
      setMode(FxMode::MASTER);
    }
  }
}

void FxManager::setMode(FxMode mode) {
  if (!initialized) {
    Serial.println("[error] FxManager not initialized");
    return;
  }

  currentMode = mode;

  switch (mode) {
    case FxMode::GAME:

      hid->disable();
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

      hid->enable();
      // Reactivate SPI for ESP control
      this->triStateSPIPins();
      this->activateSPIPins();
      delay(20);  // Give time for SPI reactivation

      oled->master();
      // Enable and configure OLED in master mode
      oled->enable();
      delay(50);  // Give extra time for display initialization

      Serial.println("Switched to MASTER mode");
      break;
    case FxMode::PROGRAMMING:
      hid->disable();
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

  File file = fileSystem->openFile(filepath);
  if (!file) {
    Serial.println("[error] Failed to open file: " + filepath);
    return;
  }

  Serial.println("Starting flash operation...");
  bool success = arduboy->flash(file);

  if (success) {
    Serial.println("[success] Flash completed successfully!");
  } else {
    Serial.println("[error] Flash operation failed");
  }

  setMode(FxMode::GAME);
}

std::array<GamesCategory, MAX_CATEGORIES> FxManager::getCategories() {
  String gamesRoot = "/arduboy";

  std::array<GamesCategory, MAX_CATEGORIES> categories = {};
  if (!initialized) {
    Serial.println("[error] FxManager not initialized");
    return categories;
  }

  if (!fileSystem) {
    Serial.println("[error] Filesystem not initialized");
    return categories;
  }

  File gamesDir = fileSystem->openFile(gamesRoot);
  if (!gamesDir || !gamesDir.isDirectory()) {
    Serial.printf("[error] Games directory not found: %s\n", gamesRoot.c_str());
    return categories;
  }

  uint8_t index = 0;
  File entry = gamesDir.openNextFile();
  while (entry && index < MAX_CATEGORIES) {
    if (!entry.isDirectory()) {
      Serial.printf("[FILE skipped] %s (%u bytes)\n", entry.name(), entry.size());
      entry = gamesDir.openNextFile();
      continue;
    }
    String childName = String(entry.name());
    String subPath;
    if (childName.length() > 0 && childName.charAt(0) == '/') {
      subPath = childName;
    } else {
      subPath = gamesRoot;
      if (!subPath.endsWith("/")) subPath += "/";
      subPath += childName;
    }
    categories[index].categoryName = childName;
    categories[index].categoryPath = subPath;
    categories[index].gameCount = 0;

    Serial.printf("[DIR CAT] %s/ %s\n", entry.name(), subPath.c_str());

    // Count games in this category
    File subRoot = fileSystem->openFile(subPath);
    if (subRoot) {
      File subFile = subRoot.openNextFile();
      while (subFile) {
        Serial.printf("[DIR GAME]  %s/\n", subFile.name());
        categories[index].gameCount++;
        subFile.close();
        subFile = subRoot.openNextFile();
      }
      subRoot.close();
    } else {
      Serial.printf("Failed to open subdirectory: %s\n", subPath.c_str());
    }

    index++;
    entry = gamesDir.openNextFile();
  }

  return categories;

}

GameInfo FxManager::getGameInfo(const String &categoryPath, uint8_t offset) {
  // mock for testing ui

  String cat = categoryPath.length() ? categoryPath : "All";
  uint8_t idx = offset + 1;
  String path = String("/games/") + cat + "/game" + String(idx) + ".hex";
  String title = cat + " Game " + String(idx);
  return GameInfo{path, title, "2023-01-01", "Dev", "Demo game.", "MIT"};

  // Placeholder implementation
  // In a real implementation, this would read metadata from the HEX file or an associated file
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
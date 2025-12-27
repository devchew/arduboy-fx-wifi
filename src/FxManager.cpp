#include "FxManager.h"
#include "UI.h"

FxManager::FxManager() {
  arduboy = nullptr;
  fileSystem = nullptr;
  oled = nullptr;
  ui = nullptr;
  hid = nullptr;
  gameLibrary = nullptr;
  initialized = false;
  currentMode = FxMode::MASTER;
}

FxManager::~FxManager() {
  delete arduboy;
  delete fileSystem;
  delete oled;
  delete ui;
  delete hid;
  delete gameLibrary;
}

bool FxManager::begin() {
  // Initialize FileSystem
  fileSystem = new FileSystemManager();
  if (!fileSystem->begin()) {
    Logger::error("Failed to initialize filesystem!");
    return false;
  }

  // Initialize ArduboyController
  arduboy = new ArduboyController();
  if (!arduboy->begin(ISP_RESET_PIN, HEX_BUFFER_SIZE)) {
    Logger::error("Failed to initialize ArduboyController!");
    return false;
  }

  hid = new HID();
  if (!hid->begin()) {
    Logger::error("Failed to initialize HID!");
    return false;
  }

  // Initialize OLEDController
  oled = new OLEDController();
  if (!oled->begin()) {
    Logger::error("Failed to initialize OLEDController!");
    return false;
  }

  ui = new UI();
  if (!ui->begin(oled->u8g2, *hid, *this)) {
    Logger::error("Failed to initialize UI!");
    return false;
  }

  gameLibrary = new GameLibrary();
  gameLibrary->begin(*fileSystem);

  gameLibrary->loadGames("/arduboy");

  initialized = true;

  setMode(FxMode::MASTER);
  return true;
}

void FxManager::update() {
  if (!initialized) {
    return;
  }

  hid->update();

  if (currentMode == FxMode::MASTER) {
    ui->update();
    if (hid->pressed(Buttons::START)) {
      setMode(FxMode::GAME);
    }
  }

  if (currentMode == FxMode::GAME) {
    if (hid->pressed(Buttons::SELECT)) {
      setMode(FxMode::MASTER);
    }
  }
}

void FxManager::setMode(FxMode mode) {
  if (!initialized) {
    Logger::error("FxManager not initialized");
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

      Logger::info("Switched to GAME mode");
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

      Logger::info("Switched to MASTER mode");
      break;
    case FxMode::PROGRAMMING:
      hid->disable();
      oled->clear();
      ui->screenFlashGame();
      oled->disable();
      delay(10);

      arduboy->powerOn();
      delay(50);  // allow bus and target to settle before probing
      Logger::info("Switched to PROGRAMMING mode");
      break;
  }
}

void FxManager::flashGame(const String& filename) {
  if (!initialized) {
    Logger::error("FxManager not initialized");
    return;
  }

  setMode(FxMode::PROGRAMMING);

  if (filename.length() == 0) {
    Logger::error("Usage: flash <filename>");
    Logger::error("Available HEX files:");
    if (fileSystem) {
      fileSystem->listHexFiles();
    }
    return;
  }

  String filepath = filename.startsWith("/") ? filename : "/" + filename;

  if (!fileSystem || !fileSystem->fileExists(filepath)) {
    Logger::error("File not found: %s\n" , filepath.c_str());
    return;
  }

  if (!fileSystem->isValidHexFile(filepath)) {
    Logger::error("Invalid HEX file: %s\n" , filepath.c_str());
    return;
  }

  if (!arduboy || !arduboy->checkConnection()) {
    Logger::error("Arduboy not connected");
    return;
  }

  File file = fileSystem->openFile(filepath);
  if (!file) {
    Logger::error("Failed to open file: %s\n" , filepath.c_str());
    return;
  }

  Logger::info("Starting flash operation...");
  bool success = arduboy->flash(file);

  if (success) {
    Logger::info("Flash completed successfully!");
  } else {
    Logger::error("Flash operation failed");
  }

  setMode(FxMode::GAME);
}

void FxManager::reset() const {
  if (!initialized) {
    Logger::error("FxManager not initialized");
    return;
  }

  if (this->currentMode == FxMode::MASTER) {
    return;
  }

  if (arduboy->reset()) {
    Logger::info("[success] Arduboy reset successfully");
  } else {
    Logger::error("Failed to reset Arduboy");
  }
}

void FxManager::printInfo() {
  if (!initialized) {
    Logger::error("FxManager not initialized");
    return;
  }

  this->setMode(FxMode::PROGRAMMING);

  delay(50);

  if (arduboy) {
    arduboy->printDeviceInfo();
  } else {
    Logger::error("ArduboyController not initialized");
  }

  if (oled) {
    oled->isInitialized() ? Logger::info("OLEDController initialized")
                          : Logger::info("OLEDController not initialized");
    oled->isMasterMode() ? Logger::info("OLEDController in MASTER mode")
                         : Logger::info("OLEDController in SLAVE mode");
  } else {
    Logger::error("OLEDController not initialized");
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
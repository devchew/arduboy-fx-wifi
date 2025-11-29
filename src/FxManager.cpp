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

  // todo: SPI line sharing with oled fight each other

  // Initialize ArduboyController
  arduboy = new ArduboyController();
  if (!arduboy->begin()) {
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

  Serial.println("Starting flash operation...");
  bool success = arduboy->flash(filepath);

  if (success) {
    Serial.println("[success] Flash completed successfully!");
  } else {
    Serial.println("[error] Flash operation failed");
  }

  setMode(FxMode::GAME);
}

std::array<GamesCategory, 20> FxManager::getCategories() {

  // mock for testing ui

  return {
      GamesCategory{"Action", "/action", 12},
      GamesCategory{"Adventure", "/adventure", 8},
      GamesCategory{"Puzzle", "/puzzle", 15},
      GamesCategory{"Arcade", "/arcade", 10},
      GamesCategory{"RPG", "/rpg", 5}
  };

  // std::array<GamesCategory, MAX_CATEGORIES> categories = {};
  // if (!initialized) {
  //   Serial.println("[error] FxManager not initialized");
  //   return categories;
  // }
  //
  // if (fileSystem) {
  //   File gamesDir = fileSystem->openFile("/games");
  //   if (!gamesDir || !gamesDir.isDirectory()) {
  //     Serial.println("[error] /games directory not found");
  //     return categories;
  //   }
  //
  //   uint8_t index = 0;
  //   File entry = gamesDir.openNextFile();
  //   while (entry && index < MAX_CATEGORIES) {
  //     if (entry.isDirectory()) {
  //       categories[index].categoryName = String(entry.name()).substring(7); // remove "/games/"
  //       categories[index].gameCount = 0;
  //
  //       // Count games in this category
  //       File categoryDir = fileSystem->openFile(String(entry.name()));
  //       File gameEntry = categoryDir.openNextFile();
  //       while (gameEntry) {
  //         if (!gameEntry.isDirectory() && fileSystem->isValidHexFile(String(gameEntry.name()))) {
  //           categories[index].gameCount++;
  //         }
  //         gameEntry = categoryDir.openNextFile();
  //       }
  //
  //       index++;
  //     }
  //     entry = gamesDir.openNextFile();
  //   }
  // } else {
  //   Serial.println("[error] Filesystem not initialized");
  // }
  //
  // return categories;

}


std::array<GameInfo, GAMES_PER_PAGE> FxManager::listGames(const String &categoryPath, uint8_t offset) {
  //mock for tesing ui

    std::array<GameInfo, GAMES_PER_PAGE> result = {};

   String cat = categoryPath.length() ? categoryPath : "All";
   for (size_t i = 0; i < GAMES_PER_PAGE; ++i) {
     uint8_t idx = offset + i + 1;
     String path = String("/games/") + cat + "/game" + String(idx) + ".hex";
     String title = cat + " Game " + String(idx);
     result[i] = GameInfo{path, title, "2023-01-01", "Dev", "Demo game.", "MIT"};
   }
   return result;

  // if (!initialized) {
  //   Serial.println("[error] FxManager not initialized");
  //   return;
  // }
  //
  // String dirPath = "/games";
  // if (category.length() > 0) {
  //   dirPath += "/" + category;
  // }
  //
  // if (!fileSystem || !fileSystem->directoryExists(dirPath)) {
  //   Serial.println("[error] Category not found: " + category);
  //   return;
  // }
  //
  // Serial.println("Listing games in category: " + (category.length() > 0 ? category : "All"));
  // Serial.println("=====================================");
  //
  // File dir = fileSystem->openFile(dirPath);
  // if (!dir) {
  //   Serial.println("[error] Failed to open directory: " + dirPath);
  //   return;
  // }
  //
  // uint8_t count = 0;
  // uint8_t listed = 0;
  // File entry = dir.openNextFile();
  // while (entry) {
  //   if (!entry.isDirectory() && fileSystem->isValidHexFile(String(entry.name()))) {
  //     if (count >= offset && listed < limit) {
  //       Serial.printf("  %s (%u bytes)\n", String(entry.name()).c_str(), entry.size());
  //       listed++;
  //     }
  //     count++;
  //   }
  //   entry = dir.openNextFile();
  // }
  //
  // if (listed == 0) {
  //   Serial.println("  No games found in this category.");
  // }
  //
  // Serial.println("=====================================");
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
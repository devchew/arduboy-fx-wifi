#ifndef FX_MANAGER_H
#define FX_MANAGER_H

#include <Arduino.h>
#include <MacroLogger.h>
#include <SPI.h>

#include <ArduboyController.h>
#include "FileSystemManager.h"
#include "OLEDController.h"
#include "HID.h"
#include "Games.h"
#include "config.h"

class UI;

enum class FxMode { GAME, MASTER, PROGRAMMING };

class ArduboyController;
class FileSystemManager;
class OLEDController;

class FxManager {
 public:
  FxManager();
  ~FxManager();
  bool begin();
  void update();
  void setMode(FxMode mode);
  FxMode getMode() const { return currentMode; }
  void flashGame(const String& filename);
  void reset();
  void printInfo();

  GameInfo getGameInfo(const String& categoryPath = "", uint8_t offset = 0) const;

  // get all categories list. Each categorry is a folder in /games
  std::array<GamesCategory, MAX_CATEGORIES> getCategories();


  FileSystemManager* fileSystem;
  FxMode currentMode;
  ArduboyController* arduboy;
  OLEDController* oled;
  UI* ui;
  HID* hid;

 private:
  bool initialized;

  void triStateSPIPins();
  void activateSPIPins();
};

#endif  // FX_MANAGER_H
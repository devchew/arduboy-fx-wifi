#ifndef FX_MANAGER_H
#define FX_MANAGER_H

#include <Arduino.h>
#include <SPI.h>

#include "ArduboyController.h"
#include "FileSystemManager.h"
#include "OLEDController.h"
#include "UI.h"
#include "HID.h"
#include "config.h"

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
  void listGames();
  void reset();
  void printInfo();

  // Accessor methods
  FileSystemManager* getFileSystemManager() const { return fileSystem; }

 private:
  bool initialized;
  FxMode currentMode;
  ArduboyController* arduboy;
  FileSystemManager* fileSystem;
  OLEDController* oled;
  UI* ui;
  HID* hid;

  void triStateSPIPins();
  void activateSPIPins();
};

#endif  // FX_MANAGER_H
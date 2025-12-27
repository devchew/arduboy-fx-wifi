#ifndef FX_MANAGER_H
#define FX_MANAGER_H

#include <Arduino.h>
#include <MacroLogger.h>
#include <SPI.h>

#include <ArduboyController.h>
#include "FileSystemManager.h"
#include "OLEDController.h"
#include "HID.h"
#include "GameLibrary.h"
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
  void reset() const;
  void printInfo();

  FileSystemManager* fileSystem;
  FxMode currentMode;
  ArduboyController* arduboy;
  OLEDController* oled;
  UI* ui;
  HID* hid;
  GameLibrary* gameLibrary;

 private:
  bool initialized;

  void triStateSPIPins();
  void activateSPIPins();
};

#endif  // FX_MANAGER_H
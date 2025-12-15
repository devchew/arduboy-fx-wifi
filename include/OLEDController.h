#ifndef OLED_CONTROLLER_H
#define OLED_CONTROLLER_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include "config.h"
#include <MacroLogger.h>

class OLEDController {
 private:
  bool initialized;
  bool isMaster;


 public:
  OLEDController();
  ~OLEDController();

  U8G2_SCREEN u8g2;

  // Initialization
  bool begin();
  void end();
  bool isInitialized() const { return initialized; }

  bool reset();

  bool slave();
  bool master();
  bool isMasterMode() const { return isMaster; }

  bool enable();
  bool disable();

  void clear();
};

#endif  // OLED_CONTROLLER_H
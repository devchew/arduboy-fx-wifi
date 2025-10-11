#ifndef OLED_CONTROLLER_H
#define OLED_CONTROLLER_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include "config.h"

class OLEDController {
 private:
  bool initialized;
  bool isMaster;

  U8X8_SSD1309_128X64_NONAME0_4W_HW_SPI u8x8;

 public:
  OLEDController();
  ~OLEDController();

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

  void helloWorld();

    void clear();

  void loop();
};

#endif  // OLED_CONTROLLER_H
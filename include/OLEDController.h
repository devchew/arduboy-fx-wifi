#ifndef OLED_CONTROLLER_H
#define OLED_CONTROLLER_H

#include <Arduino.h>
#include <U8g2lib.h>

class OLEDController {
 private:
  bool initialized;
  bool enabled;

  // Pin states
  bool resetState;
  bool enableState;

  // Internal utility functions
  void setResetPin(bool state);
  void setEnablePin(bool state);

  U8X8_SSD1306_128X64_NONAME_4W_HW_SPI u8x8;

 public:
  OLEDController();
  ~OLEDController();

  // Initialization
  bool begin();
  void end();
  bool isInitialized() const { return initialized; }

  bool reset();

  bool enable();
  bool disable();
  bool slave();
  bool master();

  void helloWorld();
};

#endif  // OLED_CONTROLLER_H
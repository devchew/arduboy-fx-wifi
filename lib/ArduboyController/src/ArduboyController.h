#ifndef ARDUBOY_CONTROLLER_H
#define ARDUBOY_CONTROLLER_H

#include <Arduino.h>
#include <MacroLogger.h>
#include <HexParser.h>
#include <ISPProgrammer.h>
#include <FS.h>

class ArduboyController {
 private:
  HexParser* hexParser;
  ISPProgrammer* ispProgrammer;
  bool initialized;
  uint8_t pinReset;

 public:
  ArduboyController();
  ~ArduboyController();

  bool begin(uint8_t pinReset, uint32_t hexBufferSize);
  bool begin(uint8_t pinReset) {
    return begin(pinReset, 32768); // 32KB for ATmega32U4
  }

  void end();

  bool checkConnection();
  bool flash(File& file);
  bool reset();
  bool powerOn();
  bool powerOff();

  void printDeviceInfo();
};

#endif  // ARDUBOY_CONTROLLER_H

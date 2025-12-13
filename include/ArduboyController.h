#ifndef ARDUBOY_CONTROLLER_H
#define ARDUBOY_CONTROLLER_H

#include <Arduino.h>

#include "HexParser.h"
#include "ISPProgrammer.h"
#include <FS.h>
#include "config.h"

class ArduboyController {
 private:
  HexParser* hexParser;
  ISPProgrammer* ispProgrammer;
  bool initialized;

 public:
  ArduboyController();
  ~ArduboyController();

  bool begin();
  void end();

  bool checkConnection();
  bool flash(File& file);
  bool reset();
  bool powerOn();
  bool powerOff();

  void printDeviceInfo();
};

#endif  // ARDUBOY_CONTROLLER_H

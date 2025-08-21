#ifndef ARDUBOY_H
#define ARDUBOY_H

#include <Arduino.h>

#include "HexParser.h"
#include "ISPProgrammer.h"

class Arduboy {
 private:
  HexParser* hexParser;
  ISPProgrammer* ispProgrammer;
  bool initialized;

 public:
  Arduboy();
  ~Arduboy();

  bool begin();
  void end();

  bool checkConnection();
  bool flash(const String& filename);
  bool reset();

  void printDeviceInfo();
};

#endif  // ARDUBOY_H

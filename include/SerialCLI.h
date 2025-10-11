#ifndef SERIALCLI_H
#define SERIALCLI_H

#include <Arduino.h>

#include "FxManager.h"
#include "config.h"

class FxManager;

class SerialCLI {
private:
 FxManager* fxManager;

public:
 SerialCLI(FxManager* fxManager = nullptr);
 ~SerialCLI();

 void update();
};

#endif  // SERIALCLI_H
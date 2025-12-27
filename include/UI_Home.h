//
// Created by Infin on 27.12.2025.
//

#ifndef ARDUBOY_FX_WIFI_UI_HOME_H
#define ARDUBOY_FX_WIFI_UI_HOME_H


#include <Arduino.h>
#include <U8g2lib.h>
#include "FxManager.h"
#include <Sprites.h>

class UI_Home {
  private:
    FxManager* fxManager = nullptr;
    //splashScreen
    int yOffset = -30;
  public:
    UI_Home(FxManager& fx);
    ~UI_Home();

    void draw();
};

#endif //ARDUBOY_FX_WIFI_UI_HOME_H
//
// Created by Infin on 27.12.2025.
//

#ifndef ARDUBOY_FX_WIFI_UI_Buttons_H
#define ARDUBOY_FX_WIFI_UI_Buttons_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "FxManager.h"
#include <Sprites.h>

class UI_ButtonsTest {
  private:
    FxManager* fxManager = nullptr;
    uint8_t exitTimer = 0;
  public:
    UI_ButtonsTest(FxManager& fx);
    ~UI_ButtonsTest();

    void draw();
};

#endif //ARDUBOY_FX_WIFI_UI_Buttons_H
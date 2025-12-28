//
// Created by Infin on 27.12.2025.
//

#ifndef ARDUBOY_FX_WIFI_UI_SETTINGS_H
#define ARDUBOY_FX_WIFI_UI_SETTINGS_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "FxManager.h"
#include <Sprites.h>

struct Option {
  String name;
  bool enabled;
};

class UI_Settings {
  private:
    FxManager* fxManager = nullptr;

    uint8_t currentPosition = 0;

    std::array<Option, 2> options = {
      Option{"Wi-Fi", false},
      Option{"Run game on startup", false}
    };

    void drawOption(const Option& option, uint8_t index);
  public:
    UI_Settings(FxManager& fx);
    ~UI_Settings();

    void draw();
};

#endif //ARDUBOY_FX_WIFI_UI_SETTINGS_H
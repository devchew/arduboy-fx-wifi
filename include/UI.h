#ifndef ARDUBOY_FX_WIFI_UI_H
#define ARDUBOY_FX_WIFI_UI_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "HID.h"
#include "FxManager.h"
#include <Sprites.h>

#include "UI_GameSelection.h"
#include "UI_Home.h"
#include "UI_ButtonssTest.h"

enum class Screen {
  NONE,
  BUTTONS_TEST,
  GAME_LIST,
  FLASH_GAME,
  HOME
};

class UI {
  private:
    U8G2_SCREEN* u8g2 = nullptr;
    HID* hid = nullptr;
    FxManager* fxManager = nullptr;
    uint8_t buttonsState = -1;
    Screen currentScreen = Screen::NONE;

    //game list screen
    UI_GameSelection* gameSelection = nullptr;
    UI_Home* home = nullptr;
    UI_ButtonsTest* buttonsTest = nullptr;

  public:
    UI();
    ~UI();
    bool begin(U8G2_SCREEN& u8g2, HID& hid, FxManager& fxManager);
    void update();
    void setScreen(Screen screen);
    void screenFlashGame() const ;
};

#endif //ARDUBOY_FX_WIFI_UI_H
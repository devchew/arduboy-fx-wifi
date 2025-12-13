#ifndef ARDUBOY_FX_WIFI_UI_H
#define ARDUBOY_FX_WIFI_UI_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "HID.h"
#include "Games.h"
#include "FxManager.h"
#include <Sprites.h>
#include <deque>

#include "UI_GameSelection.h"

enum class Screen {
  SPLASH,
  BUTTONS_TEST,
  GAME_LIST,
  FLASH_GAME,
};

class UI {
  private:
    U8G2_SCREEN* u8g2 = nullptr;
    HID* hid = nullptr;
    FxManager* fxManager = nullptr;
    uint8_t buttonsState = -1;
    Screen currentScreen = Screen::SPLASH;

    //splashScreen
    int yOffset = 0;

    //game list screen
    UI_GameSelection* gameSelection = nullptr;

    void drawTextCenter(const char* text, int8_t x_offset = 0, int8_t y_offset = 0) const;

    void screenButtonsTest() const;
    void splashScreen() ;
    void screenFlashGame() const ;

  public:
    UI();
    ~UI();
    bool begin(U8G2_SCREEN& u8g2, HID& hid, FxManager& fxManager);
    void update();
    void setScreen(Screen screen);
};

#endif //ARDUBOY_FX_WIFI_UI_H
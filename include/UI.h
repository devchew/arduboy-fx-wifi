#ifndef ARDUBOY_FX_WIFI_UI_H
#define ARDUBOY_FX_WIFI_UI_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "HID.h"
#include "Splash.h"
#include "Buttons_Sprites.h"


class FxManager;

enum class Screen {
  SPLASH,
  BUTTONS_TEST,
  GAME_LIST,
  FLASH_GAME,
};

class UI {
  private:
    U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI* u8g2;
    HID* hid;
    FxManager* fxManager;
    uint8_t buttonsState = -1;
    Screen currentScreen = Screen::SPLASH;
    uint8_t timer = 0;

    void screenButtonsTest() const;
    void splashScreen() ;
    void screenGameList() ;
    void screenFlashGame() const ;

  public:
    UI();
    ~UI();
    bool begin(U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI& u8g2, HID& hid, FxManager& fxManager);
    void update();
};

#endif //ARDUBOY_FX_WIFI_UI_H
#ifndef ARDUBOY_FX_WIFI_UI_H
#define ARDUBOY_FX_WIFI_UI_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "HID.h"
#include "Splash.h"

class UI {
  private:
    U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI* u8g2;
    void screenButtonsTest(uint8_t buttonsState) const;
    uint8_t lastButtonsState = -1;
    uint8_t currentScreen = 0;
    uint8_t timer = 0;

  public:
    UI();
    ~UI();
    bool begin(U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI& u8g2);
    void update(uint8_t buttonsState);
    void showSplashScreen();
};

#endif //ARDUBOY_FX_WIFI_UI_H
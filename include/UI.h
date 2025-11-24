#ifndef ARDUBOY_FX_WIFI_UI_H
#define ARDUBOY_FX_WIFI_UI_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "HID.h"

class UI {
  private:
    U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI* u8g2;
    void screenButtonsTest(uint8_t buttonsState) const;
    uint8_t lastButtonsState = -1;

  public:
    UI();
    ~UI();
    bool begin(U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI& u8g2);
    void update(uint8_t buttonsState);
};

#endif //ARDUBOY_FX_WIFI_UI_H
#ifndef ARDUBOY_FX_WIFI_HID_H
#define ARDUBOY_FX_WIFI_HID_H

#include <Arduino.h>
#include "config.h"
#include <MacroLogger.h>

enum class Buttons {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  A,
  B,
  SELECT,
  START
};

#define BUTTON_RIGHT_MASK   B10000000
#define BUTTON_UP_MASK      B01000000
#define BUTTON_LEFT_MASK    B00100000
#define BUTTON_DOWN_MASK    B00010000
#define BUTTON_A_MASK       B00001000
#define BUTTON_B_MASK       B00000100
#define BUTTON_SELECT_MASK  B00000010
#define BUTTON_START_MASK   B00000001

class HID {
  private:
    bool enabled;
    uint8_t buttonsState;
  public:
    HID();
    ~HID();
    bool begin();
    bool disable();
    bool enable();
    void update();
    bool pressed(Buttons button);

    /**
     * Return all buttons state
     * b(UP)(DOWN)(LEFT)(RIGHT)(A)(B)(SELECT)(START)
     * example: A button pressed > b00001000
     */
    uint8_t getButtons();
};

#endif //ARDUBOY_FX_WIFI_HID_H
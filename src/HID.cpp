#include "HID.h"


HID::HID() {
  enabled = false;
  buttonsState = 0;
}

HID::~HID() {
  this->disable();
}

bool HID::begin() {
  this->enable();
  return true;
}

bool HID::disable() {
  // disable pullups on disable state
  pinMode(BUTTON_PIN_A, INPUT);
  pinMode(BUTTON_PIN_B, INPUT);
  pinMode(BUTTON_PIN_SELECT, INPUT);
  pinMode(BUTTON_PIN_START, INPUT);
  pinMode(BUTTON_PIN_UP, INPUT);
  pinMode(BUTTON_PIN_DOWN, INPUT);
  pinMode(BUTTON_PIN_LEFT, INPUT);
  pinMode(BUTTON_PIN_RIGHT, INPUT);
  enabled = false;
  buttonsState = 0;

  return true;
}

bool HID::enable() {
  // enable pullups on enable state
  pinMode(BUTTON_PIN_A, INPUT_PULLUP);
  pinMode(BUTTON_PIN_B, INPUT_PULLUP);
  pinMode(BUTTON_PIN_SELECT, INPUT_PULLUP);
  pinMode(BUTTON_PIN_START, INPUT_PULLUP);
  pinMode(BUTTON_PIN_UP, INPUT_PULLUP);
  pinMode(BUTTON_PIN_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_PIN_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_PIN_RIGHT, INPUT_PULLUP);
  return true;
}

uint8_t HID::getButtons() {
  return buttonsState;
}

bool HID::pressed(Buttons button) {
  switch (button) {
    case Buttons::UP:
      return (buttonsState & BUTTON_UP_MASK) != 0;
    case Buttons::DOWN:
      return (buttonsState & BUTTON_DOWN_MASK) != 0;
    case Buttons::LEFT:
      return (buttonsState & BUTTON_LEFT_MASK) != 0;
    case Buttons::RIGHT:
      return (buttonsState & BUTTON_RIGHT_MASK) != 0;
    case Buttons::A:
      return (buttonsState & BUTTON_A_MASK) != 0;
    case Buttons::B:
      return (buttonsState & BUTTON_B_MASK) != 0;
    case Buttons::SELECT:
      return (buttonsState & BUTTON_SELECT_MASK) != 0;
    case Buttons::START:
      return (buttonsState & BUTTON_START_MASK) != 0;
    default:
      return false;
  }
}
/**
 * Return all buttons state
 * b(UP)(DOWN)(LEFT)(RIGHT)(A)(B)(SELECT)(START)
 * example: A button pressed > b00001000
 */
void HID::update() {
  uint8_t newButtonsState = 0;
  newButtonsState |= (digitalRead(BUTTON_PIN_UP) == LOW) ? BUTTON_UP_MASK : 0;
  newButtonsState |= (digitalRead(BUTTON_PIN_DOWN) == LOW) ? BUTTON_DOWN_MASK : 0;
  newButtonsState |= (digitalRead(BUTTON_PIN_LEFT) == LOW) ? BUTTON_LEFT_MASK : 0;
  newButtonsState |= (digitalRead(BUTTON_PIN_RIGHT) == LOW) ? BUTTON_RIGHT_MASK : 0;
  newButtonsState |= (digitalRead(BUTTON_PIN_A) == LOW) ? BUTTON_A_MASK : 0;
  newButtonsState |= (digitalRead(BUTTON_PIN_B) == LOW) ? BUTTON_B_MASK : 0;
  newButtonsState |= (digitalRead(BUTTON_PIN_SELECT) == LOW) ? BUTTON_SELECT_MASK : 0;
  newButtonsState |= (digitalRead(BUTTON_PIN_START) == LOW) ? BUTTON_START_MASK : 0;

  if (newButtonsState != buttonsState) {
    buttonsState = newButtonsState;
    Serial.println("Buttons state");
    Serial.print("b");
    Serial.println(buttonsState, BIN);
  }
}





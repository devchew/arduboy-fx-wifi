#include "UI.h"

UI::UI() {
  u8g2 = nullptr;
  lastButtonsState = -1;
}

UI::~UI() {

}

bool UI::begin(U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI& u8x8) {
  u8g2 = &u8x8;
  return true;
}

void UI::screenButtonsTest(uint8_t buttonsState) const {
  u8g2->setFont(u8g2_font_4x6_tr);
  u8g2->drawStr(101, 25, "A");
  u8g2->drawStr(85, 40, "B");
  u8g2->drawStr(28, 60, "START");
  u8g2->drawStr(68, 60, "SELECT");

  //UP
  if ((buttonsState & BUTTON_UP_MASK) != 0) {
    u8g2->drawBox(22, 10, 14, 14);
  } else {
    u8g2->drawFrame(22, 10, 14, 14);
  }

  //LEFT
  if ((buttonsState & BUTTON_LEFT_MASK) != 0) {
    u8g2->drawBox(9, 23, 14, 14);
  } else {
    u8g2->drawFrame(9, 23, 14, 14);
  }
  //RIGHT
  if ((buttonsState & BUTTON_RIGHT_MASK) != 0) {
    u8g2->drawBox(35, 23, 14, 14);
  } else {
    u8g2->drawFrame(35, 23, 14, 14);
  }

  //DOWN
  if ((buttonsState & BUTTON_DOWN_MASK) != 0) {
    u8g2->drawBox(22, 36, 14, 14);
  } else {
    u8g2->drawFrame(22, 36, 14, 14);
  }

  //A
  if ((buttonsState & BUTTON_A_MASK) != 0) {
    u8g2->drawFilledEllipse(102, 22, 7, 7);
  } else {
    u8g2->drawEllipse(102, 22, 7, 7);
  }

  //B
  if ((buttonsState & BUTTON_B_MASK) != 0) {
    u8g2->drawFilledEllipse(86, 37, 7, 7);
  } else {
    u8g2->drawEllipse(86, 37, 7, 7);
  }

  //START
  if ((buttonsState & BUTTON_START_MASK) != 0) {
    u8g2->drawBox(24, 52, 27, 11);
  } else {
    u8g2->drawFrame(24, 52, 27, 11);
  }

  //SELECT
  if ((buttonsState & BUTTON_SELECT_MASK) != 0) {
    u8g2->drawBox(64, 52, 31, 11);
  } else {
    u8g2->drawFrame(64, 52, 31, 11);
  }
}

void UI::update(uint8_t buttonsState) {
  if (buttonsState == lastButtonsState) {
    return; // No change in button state
  }
  u8g2->firstPage();
  do {
    this->screenButtonsTest(buttonsState);
    yield();
  } while ( u8g2->nextPage() );

}




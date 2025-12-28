#include "UI_ButtonssTest.h"
#include "UI.h"

UI_ButtonsTest::UI_ButtonsTest(FxManager &fx) {
  fxManager = &fx;
}

UI_ButtonsTest::~UI_ButtonsTest() {
  fxManager = nullptr;
}

void UI_ButtonsTest::draw() {
  auto buttonsState = fxManager->hid->getButtons();
  fxManager->oled->u8g2.clearBuffer();
  fxManager->oled->u8g2.setFont(u8g2_font_4x6_tr);
  fxManager->oled->u8g2.drawStr(55, 50, "Start");

  fxManager->oled->u8g2.drawStr(48, 59, "Select");

  //UP
  if ((buttonsState & BUTTON_UP_MASK) != 0) {
    fxManager->oled->u8g2.drawXBMP(21, 18, 7, 9, sprite_up_press);
  } else {
    fxManager->oled->u8g2.drawXBMP(21, 18, 7, 9, sprite_up);
  }

  //LEFT
  if ((buttonsState & BUTTON_LEFT_MASK) != 0) {
    fxManager->oled->u8g2.drawXBMP(14, 25, 9, 7, sprite_left_press);
  } else {
    fxManager->oled->u8g2.drawXBMP(14, 25, 9, 7, sprite_left);
  }
  //RIGHT
  if ((buttonsState & BUTTON_RIGHT_MASK) != 0) {
    fxManager->oled->u8g2.drawXBMP(26, 25, 9, 7, sprite_right_press);
  } else {
    fxManager->oled->u8g2.drawXBMP(26, 25, 9, 7, sprite_right);
  }

  //DOWN
  if ((buttonsState & BUTTON_DOWN_MASK) != 0) {
    fxManager->oled->u8g2.drawXBMP(21, 30, 7, 9, sprite_down_press);
  } else {
    fxManager->oled->u8g2.drawXBMP(21, 30, 7, 9, sprite_down);
  }

  //A
  if ((buttonsState & BUTTON_A_MASK) != 0) {
    fxManager->oled->u8g2.drawXBMP(106, 21, 11, 11, sprite_a_press);
  } else {
    fxManager->oled->u8g2.drawXBMP(106, 21, 11, 11, sprite_a);
  }

  //B
  if ((buttonsState & BUTTON_B_MASK) != 0) {
    fxManager->oled->u8g2.drawXBMP(94, 33, 11, 11, sprite_b_press);
    exitTimer++;
  } else {
    fxManager->oled->u8g2.drawXBMP(94, 33, 11, 11, sprite_b);
    exitTimer = 0;
  }

  //START
  if ((buttonsState & BUTTON_START_MASK) != 0) {
    fxManager->oled->u8g2.drawFilledEllipse(49, 47, 3, 3);
  } else {
    fxManager->oled->u8g2.drawEllipse(49, 47, 3, 3);
  }

  //SELECT
  if ((buttonsState & BUTTON_SELECT_MASK) != 0) {
    fxManager->oled->u8g2.drawFilledEllipse(42, 56, 3, 3);
  } else {
    fxManager->oled->u8g2.drawEllipse(42, 56, 3, 3);
  }

  fxManager->oled->u8g2.drawLine(1, 1, exitTimer + 1, 1);

  if (exitTimer >= 126) {
    fxManager->ui->setScreen(Screen::HOME);
    exitTimer = 0;
  }

  fxManager->oled->u8g2.sendBuffer();
}

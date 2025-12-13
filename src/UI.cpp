#include "UI.h"

UI::UI() {}
UI::~UI() = default;

bool UI::begin(U8G2_SCREEN& u8x8, HID& hidInstance, FxManager& fxManagerInstance) {
  u8g2 = &u8x8;
  hid = &hidInstance;
  fxManager = &fxManagerInstance;

  return true;
}

void UI::screenButtonsTest() const {
  u8g2->clearBuffer();
  u8g2->setFont(u8g2_font_4x6_tr);
  u8g2->drawStr(55, 50, "Start");

  u8g2->drawStr(48, 59, "Sellect");

  //UP
  if ((buttonsState & BUTTON_UP_MASK) != 0) {
    u8g2->drawXBMP(21, 18, 7, 9, sprite_up_press);
  } else {
    u8g2->drawXBMP(21, 18, 7, 9, sprite_up);
  }

  //LEFT
  if ((buttonsState & BUTTON_LEFT_MASK) != 0) {
    u8g2->drawXBMP(14, 25, 9, 7, sprite_left_press);
  } else {
    u8g2->drawXBMP(14, 25, 9, 7, sprite_left);
  }
  //RIGHT
  if ((buttonsState & BUTTON_RIGHT_MASK) != 0) {
    u8g2->drawXBMP(26, 25, 9, 7, sprite_right_press);
  } else {
    u8g2->drawXBMP(26, 25, 9, 7, sprite_right);
  }

  //DOWN
  if ((buttonsState & BUTTON_DOWN_MASK) != 0) {
    u8g2->drawXBMP(21, 30, 7, 9, sprite_down_press);
  } else {
    u8g2->drawXBMP(21, 30, 7, 9, sprite_down);
  }

  //A
  if ((buttonsState & BUTTON_A_MASK) != 0) {
    u8g2->drawXBMP(106, 21, 11, 11, sprite_a_press);
  } else {
    u8g2->drawXBMP(106, 21, 11, 11, sprite_a);
  }

  //B
  if ((buttonsState & BUTTON_B_MASK) != 0) {
    u8g2->drawXBMP(94, 33, 11, 11, sprite_b_press);
  } else {
    u8g2->drawXBMP(94, 33, 11, 11, sprite_b);
  }

  //START
  if ((buttonsState & BUTTON_START_MASK) != 0) {
    u8g2->drawFilledEllipse(49, 47, 3, 3);
  } else {
    u8g2->drawEllipse(49, 47, 3, 3);
  }

  //SELECT
  if ((buttonsState & BUTTON_SELECT_MASK) != 0) {
    u8g2->drawFilledEllipse(42, 56, 3, 3);
  } else {
    u8g2->drawEllipse(42, 56, 3, 3);
  }

  u8g2->sendBuffer();
}

void UI::splashScreen() {
  if (millis() % 10 == 0) {
    yOffset++;
    u8g2->clearBuffer();
    int position = -128 + yOffset;
    u8g2->drawXBMP(0, position > 0 ? 0 : position , 128, 64, splashImage);
    if (yOffset > 200) {
      setScreen(Screen::GAME_LIST);
      yOffset = 0;
    }
    u8g2->sendBuffer();
  }
}

void UI::drawTextCenter(const char *text, int8_t x_offset, int8_t y_offset) const {
  u8g2->setFont(u8g2_font_profont15_tr);
  uint8_t textWidth = u8g2->getStrWidth(text);
  u8g2->drawStr(((128 - textWidth) / 2) + x_offset, 34 + y_offset, text);
}

void UI::setScreen(Screen screen) {

  if (screen == Screen::GAME_LIST) {
    gameSelection = new UI_GameSelection(*fxManager);
  } else {
    if (gameSelection != nullptr) {
      delete gameSelection;
      gameSelection = nullptr;
    }
  }

  currentScreen = screen;

}

void UI::screenFlashGame() const {
  u8g2->clearBuffer();
  u8g2->setFont(u8g2_font_profont15_tr);
  u8g2->drawStr(18, 34, "Flashing game");
  u8g2->sendBuffer();
}


void UI::update() {
  buttonsState = hid->getButtons();
  if (hid->pressed(Buttons::SELECT)) {
    setScreen(Screen::BUTTONS_TEST);
    return;
  }
  if (hid->pressed(Buttons::B)) {
    setScreen(Screen::GAME_LIST);
    return;
  }
  if (hid->pressed(Buttons::A)) {
    setScreen(Screen::FLASH_GAME);
    return;
  }
  switch (currentScreen) {
    case Screen::SPLASH:
      this->splashScreen();
      break;
    case Screen::BUTTONS_TEST:
      this->screenButtonsTest();
      break;
    case Screen::GAME_LIST:
      if (gameSelection != nullptr) {
        gameSelection->draw();
      }
      break;
    case Screen::FLASH_GAME:
      this->screenFlashGame();
      break;
    default:
      setScreen(Screen::SPLASH);
      break;
  }
}




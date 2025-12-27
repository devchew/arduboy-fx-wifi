#include "UI.h"

UI::UI() {}
UI::~UI() = default;

bool UI::begin(U8G2_SCREEN& u8x8, HID& hidInstance, FxManager& fxManagerInstance) {
  u8g2 = &u8x8;
  hid = &hidInstance;
  fxManager = &fxManagerInstance;

  setScreen(Screen::HOME);
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

void UI::setScreen(Screen screen) {
  if (screen == currentScreen) {
    return;
  }

  // clean up
  if (currentScreen == Screen::HOME) {
    if (home != nullptr) {
      delete home;
      home = nullptr;
    }
  }
  if (currentScreen == Screen::GAME_LIST) {
    if (gameSelection != nullptr) {
      delete gameSelection;
      gameSelection = nullptr;
    }
  }

  // initialize new screen
  if (screen == Screen::HOME) {
    home = new UI_Home(*fxManager);
  }
  if (screen == Screen::GAME_LIST) {
    gameSelection = new UI_GameSelection(*fxManager);
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
  switch (currentScreen) {
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
    case Screen::HOME:
      if (home != nullptr) {
        home->draw();
      }
      break;
    default:
      setScreen(Screen::HOME);
      break;
  }
}




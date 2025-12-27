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
  if (currentScreen == Screen::BUTTONS_TEST) {
    if (buttonsTest != nullptr) {
      delete buttonsTest;
      buttonsTest = nullptr;
    }
  }

  // initialize new screen
  if (screen == Screen::HOME) {
    home = new UI_Home(*fxManager);
  }
  if (screen == Screen::GAME_LIST) {
    gameSelection = new UI_GameSelection(*fxManager);
  }
  if (screen == Screen::BUTTONS_TEST) {
    buttonsTest = new UI_ButtonsTest(*fxManager);
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
      if (buttonsTest != nullptr) {
        buttonsTest->draw();
      }
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




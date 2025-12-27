#include "UI.h"

UI::UI() {}
UI::~UI() {
  if (home != nullptr) {
    delete home;
    home = nullptr;
  }
  if (gameSelection != nullptr) {
    delete gameSelection;
    gameSelection = nullptr;
  }
  if (buttonsTest != nullptr) {
    delete buttonsTest;
    buttonsTest = nullptr;
  }
  if (settings != nullptr) {
    delete settings;
    settings = nullptr;
  }
}

bool UI::begin(FxManager& fxManagerInstance) {
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
  if (currentScreen == Screen::SETTINGS) {
    if (settings != nullptr) {
      delete settings;
      settings = nullptr;
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
  if (screen == Screen::SETTINGS) {
    settings = new UI_Settings(*fxManager);
  }

  currentScreen = screen;

}

void UI::screenFlashGame() const {
  fxManager->oled->u8g2.clearBuffer();
  fxManager->oled->u8g2.setFont(u8g2_font_profont15_tr);
  fxManager->oled->u8g2.drawStr(18, 34, "Flashing game");
  fxManager->oled->u8g2.sendBuffer();
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
    case Screen::SETTINGS:
      if (settings != nullptr) {
        settings->draw();
      }
      break;
    default:
      setScreen(Screen::HOME);
      break;
  }
}




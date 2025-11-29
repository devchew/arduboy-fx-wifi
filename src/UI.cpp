#include "UI.h"


UI::UI() {
  u8g2 = nullptr;
  buttonsState = -1;
  hid = nullptr;
  fxManager = nullptr;
  currentScreen = Screen::SPLASH;
}

UI::~UI() = default;

bool UI::begin(U8G2_SCREEN& u8x8, HID& hidInstance, FxManager& fxManagerInstance) {
  u8g2 = &u8x8;
  hid = &hidInstance;
  fxManager = &fxManagerInstance;

  // initially load games
  categories = fxManager->getCategories();
  loadedGames = fxManager->listGames(categories[currentCategoryIndex].categoryPath, currentGameOffset);
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
      currentScreen = Screen::GAME_LIST; // Switch to buttons test screen
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

void UI::drawGameSplashScreen(const GameInfo& game, int8_t x_offset, int8_t y_offset) const {
  u8g2->setFont(u8g2_font_profont15_tr);
  uint8_t textWidth = u8g2->getStrWidth(game.title.c_str());
  u8g2->drawStr(((128 - textWidth) / 2) + x_offset, 20 + y_offset, game.title.c_str());

  textWidth = u8g2->getStrWidth(game.author.c_str());
  u8g2->drawStr(((128 - textWidth) / 2) + x_offset, 40 + y_offset, game.author.c_str());
}


void UI::screenGameList() {
  if (hid->pressed(Buttons::UP)) {
    this->direction = 1;
  }
  if (hid->pressed(Buttons::DOWN)) {
    this->direction = 2;
  }

  if (millis() % 5 != 0) {
    return;
  }

  u8g2->clearBuffer();
  u8g2->setFont(u8g2_font_profont15_tr);

  switch (direction) {
    case 1: // UP
      yOffset++;
      if (yOffset == 0) {
        direction = 0;
      }
      if (gamePosition <= 0 && yOffset >= 21) {
        // prevent moving up if at the top of the list
        // but let the offset reset smoothly by continuing the animation
        // and reversing the direction
        direction = 2;
        yOffset = 21;
      }
      if (yOffset >= 80) {
        yOffset = 0;
        direction = 0;
        gamePosition--;
      }
      break;
    case 2: // DOWN
      yOffset--;
      if (yOffset == 0) {
        direction = 0;
      }
      if (yOffset <= -80) {
        yOffset = 0;
        direction = 0;
        gamePosition++;
      }
      break;
    default:
      yOffset = 0;
      break;
  }

  if (direction == 0) {
    this->drawGameSplashScreen(loadedGames[gamePosition]);
  }

  if (gamePosition > 0) {
    this->drawGameSplashScreen(loadedGames[gamePosition - 1], 0, -80 + yOffset);
  }

  this->drawGameSplashScreen(loadedGames[gamePosition], 0, 0 + yOffset);
  this->drawGameSplashScreen(loadedGames[gamePosition + 1], 0, 80 + yOffset);

  u8g2->sendBuffer();
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
    currentScreen = Screen::BUTTONS_TEST;
    return;
  }
  if (hid->pressed(Buttons::B)) {
    currentScreen = Screen::GAME_LIST;
    return;
  }
  if (hid->pressed(Buttons::A)) {
    currentScreen = Screen::FLASH_GAME;
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
      this->screenGameList();
      break;
    case Screen::FLASH_GAME:
      this->screenFlashGame();
      break;
    default:
      currentScreen = Screen::SPLASH;
      break;
  }
}




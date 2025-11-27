#include "UI.h"

UI::UI() {
  u8g2 = nullptr;
  buttonsState = -1;
  hid = nullptr;
  fxManager = nullptr;
  currentScreen = Screen::SPLASH;
  timer = 0;
}

UI::~UI() = default;

bool UI::begin(U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI& u8x8, HID& hidInstance, FxManager& fxManagerInstance) {
  u8g2 = &u8x8;
  hid = &hidInstance;
  fxManager = &fxManagerInstance;
  return true;
}

void UI::screenButtonsTest() const {
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
}

void UI::splashScreen() {
  if (millis() % 10 == 0) {
    timer++;
  }
  int position = -128 + timer;
  u8g2->drawXBMP(0, position > 0 ? 0 : position , 128, 64, splashImage);
  if (timer > 200) {
    currentScreen = Screen::BUTTONS_TEST; // Switch to buttons test screen
    timer = 0;
  }
}

void UI::screenGameList() {

}

void UI::screenFlashGame() const {
  u8g2->setFont(u8g2_font_profont15_tr);
  u8g2->drawStr(18, 34, "Flashing game");
}


void UI::update() {
  buttonsState = hid->getButtons();
  u8g2->firstPage();
  do {
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
    yield();
  } while ( u8g2->nextPage() );

}




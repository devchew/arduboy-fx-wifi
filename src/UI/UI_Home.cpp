#include "UI_Home.h"

#include "UI.h"

UI_Home::UI_Home(FxManager &fx) {
  fxManager = &fx;
}

UI_Home::~UI_Home() {
  fxManager = nullptr;
}

void UI_Home::draw() {
  fxManager->oled->u8g2.clearBuffer();
  if (yOffset <= 0) {
    if (millis() % 10 == 0) {
      yOffset++;
      fxManager->oled->u8g2.drawXBMP(20, yOffset, 88, 25, sprite_splash);
      fxManager->oled->u8g2.sendBuffer();
    }
    return;
  }

  fxManager->oled->u8g2.setFontMode(1);
  fxManager->oled->u8g2.setBitmapMode(1);
  fxManager->oled->u8g2.setFont(u8g2_font_4x6_tr);

  fxManager->oled->u8g2.drawXBMP(2, 57, 3, 5, sprite_left_mini);
  fxManager->oled->u8g2.drawXBMP(123, 57, 3, 5, sprite_right_mini);
  fxManager->oled->u8g2.drawStr(102, 62, "Games");
  fxManager->oled->u8g2.drawStr(7, 62, "Settings");
  fxManager->oled->u8g2.drawXBMP(20, 2, 88, 25, sprite_splash);

  if (fxManager->currentFlashedGame != nullptr) {
    fxManager->oled->u8g2.drawXBMP(28, 32, 11, 7, sprite_action_right_mini);
    fxManager->oled->u8g2.drawStr(42, 38, "Play last game");
    fxManager->oled->u8g2.setFont(u8g2_font_6x10_tr);
    fxManager->oled->u8g2.drawStr(31, 49, fxManager->currentFlashedGame->title.c_str());
  }
  fxManager->oled->u8g2.sendBuffer();

  if (fxManager->hid->pressed(Buttons::RIGHT)) {
    delay(200); // simple debounce
    fxManager->ui->setScreen(Screen::GAME_LIST);
    return;
  }
  if (fxManager->hid->pressed(Buttons::LEFT)) {
    delay(200); // simple debounce
    fxManager->ui->setScreen(Screen::SETTINGS);
    return;
  }

  if (fxManager->hid->pressed(Buttons::A)) {
    delay(200); // simple debounce
    fxManager->setMode(FxMode::GAME);
    return;
  }


}
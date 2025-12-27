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
  // Layer 13 copy
  fxManager->oled->u8g2.setFont(u8g2_font_4x6_tr);
  fxManager->oled->u8g2.drawStr(42, 38, "Play last game");

  // left copy
  fxManager->oled->u8g2.drawXBMP(2, 57, 3, 5, sprite_left_mini);

  // right copy
  fxManager->oled->u8g2.drawXBMP(123, 57, 3, 5, sprite_right_mini);

  // Layer 13 copy
  fxManager->oled->u8g2.drawStr(102, 62, "Games");

  // right_button
  fxManager->oled->u8g2.drawXBMP(28, 32, 11, 7, sprite_action_right_mini);

  // Layer 13
  fxManager->oled->u8g2.drawStr(7, 62, "Settings");

  // splish
  fxManager->oled->u8g2.drawXBMP(20, 2, 88, 25, sprite_splash);

  // Layer 13 copy
  fxManager->oled->u8g2.setFont(u8g2_font_6x10_tr);
  fxManager->oled->u8g2.drawStr(31, 49, "Arduventure");

  fxManager->oled->u8g2.sendBuffer();

  if (fxManager->hid->pressed(Buttons::RIGHT)) {
    delay(200); // simple debounce
    fxManager->ui->setScreen(Screen::GAME_LIST);
  }
  if (fxManager->hid->pressed(Buttons::LEFT)) {
    delay(200); // simple debounce
    fxManager->ui->setScreen(Screen::BUTTONS_TEST);
  }

  if (fxManager->hid->pressed(Buttons::A)) {
    delay(200); // simple debounce
    fxManager->setMode(FxMode::GAME);
  }


}
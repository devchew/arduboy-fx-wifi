#include "UI_Settings.h"
#include "UI.h"

UI_Settings::UI_Settings(FxManager &fx) {
  fxManager = &fx;
}
UI_Settings::~UI_Settings() {
  fxManager = nullptr;
}

void UI_Settings::drawOption(const Option &option, uint8_t index) {
  if (index == currentPosition) {
    fxManager->oled->u8g2.drawEllipse(4, 18 + (index * 9), 1, 1);
  }
  fxManager->oled->u8g2.setFont(u8g2_font_4x6_tr);
  fxManager->oled->u8g2.drawStr(9, 21 + (index * 9), option.name.c_str());
  fxManager->oled->u8g2.drawXBMP(102, 14 + (index * 9), 17, 8, option.enabled ? sprite_toggle_on : sprite_toggle_off);
}

void UI_Settings::draw() {
  if (fxManager->hid->pressed(Buttons::B)) {
    fxManager->ui->setScreen(Screen::HOME);
    return;
  }

  if (fxManager->hid->pressed(Buttons::A)) {
    delay(200); // simple debounce
    options[currentPosition].enabled = !options[currentPosition].enabled;
  }

  if (fxManager->hid->pressed(Buttons::UP)) {
    delay(200); // simple debounce
    if (currentPosition > 0) {
      currentPosition--;
    }
  }
  if (fxManager->hid->pressed(Buttons::DOWN)) {
    delay(200); // simple debounce
    if (currentPosition < 1) {
      currentPosition++;
    }
  }

  fxManager->oled->u8g2.clearBuffer();
  fxManager->oled->u8g2.setFontMode(1);
  fxManager->oled->u8g2.setBitmapMode(1);

  // Titlebar
  fxManager->oled->u8g2.setFont(u8g2_font_6x10_tr);
  fxManager->oled->u8g2.drawStr(3, 10, "Settings");

  // Navbar
  fxManager->oled->u8g2.setFont(u8g2_font_4x6_tr);
  fxManager->oled->u8g2.drawXBMP(3, 58, 5, 3, sprite_up_mini);
  fxManager->oled->u8g2.drawStr(11, 62, "Select");
  fxManager->oled->u8g2.drawXBMP(36, 58, 5, 3, sprite_down_mini);
  fxManager->oled->u8g2.drawStr(74, 62, "Back");
  fxManager->oled->u8g2.drawXBMP(91, 56, 11, 7, sprite_action_buttons_mini);
  fxManager->oled->u8g2.drawStr(103, 62, "Toggle");

  // Options
  for (uint8_t i = 0; i < options.size(); i++) {
    drawOption(options[i], i);
  }

  fxManager->oled->u8g2.sendBuffer();
}
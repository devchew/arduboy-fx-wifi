//
// Created by Infin on 13.12.2025.
//

#include "UI_GameSelection.h"

#include "UI.h"


UI_GameSelection::UI_GameSelection(FxManager& fx) {
  fxManager = &fx;
  categories = fxManager->getCategories();
  // loadedGames = {
  //   GameInfo(),
  //   fxManager->getGameInfo(categories[currentCategoryIndex].categoryPath, 0),
  //   fxManager->getGameInfo(categories[currentCategoryIndex].categoryPath, 1),
  // };

  currentGame = fxManager->getGameInfo(categories[currentCategoryIndex].categoryPath, 0);
}

UI_GameSelection::~UI_GameSelection() {
  fxManager = nullptr;
}

void UI_GameSelection::drawGameSplashScreen(const GameInfo& game, int8_t x_offset, int8_t y_offset) const {
  fxManager->oled->u8g2.setFont(u8g2_font_profont15_tr);
  uint8_t textWidth = fxManager->oled->u8g2.getStrWidth(game.title.c_str());
  fxManager->oled->u8g2.drawStr(((128 - textWidth) / 2) + x_offset, 20 + y_offset, game.title.c_str());

  textWidth = fxManager->oled->u8g2.getStrWidth(game.author.c_str());
  fxManager->oled->u8g2.drawStr(((128 - textWidth) / 2) + x_offset, 40 + y_offset, game.author.c_str());
}

void UI_GameSelection::drawCategoryScreen(const GamesCategory &category, int8_t x_offset, int8_t y_offset) const {
  fxManager->oled->u8g2.setFont(u8g2_font_profont15_tr);
  uint8_t textWidth = fxManager->oled->u8g2.getStrWidth(category.categoryName.c_str());
  fxManager->oled->u8g2.drawStr(((128 - textWidth) / 2) + x_offset, 20 + y_offset, category.categoryName.c_str());

  String gameCountText = String(category.gameCount) + " games";

  textWidth = fxManager->oled->u8g2.getStrWidth(gameCountText.c_str());
  fxManager->oled->u8g2.drawStr(((128 - textWidth) / 2) + x_offset, 40 + y_offset, gameCountText.c_str());
}

void UI_GameSelection::draw() {
  fxManager->oled->u8g2.clearBuffer();
  fxManager->oled->u8g2.setFont(u8g2_font_profont15_tr);

  if (currentGameIndex > 0) {
    this->drawGameSplashScreen(currentGame);
  } else {
    this->drawCategoryScreen(categories[currentCategoryIndex]);
  }

  fxManager->oled->u8g2.sendBuffer();

  if (fxManager->hid->pressed(Buttons::RIGHT) && categories[currentCategoryIndex +1].categoryName.length() >0) {
    delay(200); // simple debounce
    currentCategoryIndex ++;
    currentGameIndex = 0;
    currentGame = fxManager->getGameInfo(categories[currentCategoryIndex].categoryPath, currentGameIndex -1);
  }

  if (fxManager->hid->pressed(Buttons::LEFT) && currentCategoryIndex > 0) {
    delay(200); // simple debounce
    currentCategoryIndex --;
    currentGameIndex = 0;
    currentGame = fxManager->getGameInfo(categories[currentCategoryIndex].categoryPath, currentGameIndex -1);
  }

  if (fxManager->hid->pressed(Buttons::DOWN)) {
    delay(200); // simple debounce
    currentGameIndex ++;
    currentGame = fxManager->getGameInfo(categories[currentCategoryIndex].categoryPath, currentGameIndex -1);
  }

  if (fxManager->hid->pressed(Buttons::UP) && currentGameIndex > 0) {
    delay(200); // simple debounce
    currentGameIndex --;
    currentGame = fxManager->getGameInfo(categories[currentCategoryIndex].categoryPath, currentGameIndex -1);
  }

  if (fxManager->hid->pressed(Buttons::A) && currentGame.filePath.length() > 0 && currentGameIndex > 0) {
    delay(200); // simple debounce
    fxManager->flashGame(currentGame.filePath.c_str());
  }

}

// void UI_GameSelection::draw() {
//   if (fxManager->hid->pressed(Buttons::UP)) {
//     this->direction = 1;
//   }
//   if (fxManager->hid->pressed(Buttons::DOWN)) {
//     this->direction = 2;
//   }
//
//   if (millis() % 5 != 0) {
//     return;
//   }
//
//   fxManager->oled->u8g2.clearBuffer();
//   fxManager->oled->u8g2.setFont(u8g2_font_profont15_tr);
//
//   switch (direction) {
//     case 1: // UP
//       yOffset++;
//       if (yOffset == 0) {
//         direction = 0;
//       }
//       if (currentGameIndex <= 0 && yOffset >= 21) {
//         // prevent moving up if at the top of the list
//         // but let the offset reset smoothly by continuing the animation
//         // and reversing the direction
//         direction = 2;
//         yOffset = 21;
//       }
//       if (yOffset >= 80) {
//         yOffset = 0;
//         direction = 0;
//         currentGameIndex--;
//
//         // check if we need to load previous games
//         // [3,4,5] -> [2,3,4] and 2 need to be loaded
//         loadedGames[2] = loadedGames[1];
//         loadedGames[1] = loadedGames[0];
//         if (currentGameIndex >= 0) {
//           loadedGames[0] = fxManager->getGameInfo(categories[currentCategoryIndex].categoryPath, currentGameIndex -1);
//         }
//       }
//       break;
//     case 2: // DOWN
//       yOffset--;
//       if (yOffset == 0) {
//         direction = 0;
//       }
//       if (currentGameIndex >= categories[currentCategoryIndex].gameCount - 1 && yOffset < -21) {
//         // prevent moving down if at the end of the list
//         // but let the offset reset smoothly by continuing the animation
//         // and reversing the direction
//         direction = 1;
//         yOffset = -21;
//       }
//       if (yOffset <= -80) {
//         yOffset = 0;
//         direction = 0;
//         currentGameIndex++;
//
//         // check if we need to load more games
//         // [3,4,5] -> [4,5,6] and 6 need to be loaded
//         loadedGames[0] = loadedGames[1];
//         loadedGames[1] = loadedGames[2];
//         if (currentGameIndex <= categories[currentCategoryIndex].gameCount) {
//           loadedGames[2] = fxManager->getGameInfo(categories[currentCategoryIndex].categoryPath, currentGameIndex +1);
//         }
//
//       }
//       break;
//     default:
//       yOffset = 0;
//       break;
//   }
//
//   fxManager->oled->u8g2.setFont(u8g2_font_4x6_tr);
//   // debug values
//   fxManager->oled->u8g2.drawStr(1, 63, ("gi:" + String(currentGameIndex) + " ci:" + (categories[currentCategoryIndex].categoryPath)).c_str());
//
//   if (fxManager->fileSystem->isInitialized()) {
//     fxManager->oled->u8g2.drawXBMP(115, 1, 11, 8, sprite_sd_card);
//   }
//
//
//   if (currentGameIndex > 0) {
//     this->drawGameSplashScreen(loadedGames[0], 0, -80 + yOffset);
//   }
//
//   if (currentGameIndex == 0) {
//     this->drawCategoryScreen(categories[currentGameIndex], 0, 0 + yOffset);
//   } else {
//     this->drawGameSplashScreen(loadedGames[1], 0, 0 + yOffset);
//   }
//   // this->drawGameSplashScreen(loadedGames[1], 0, 0 + yOffset);
//   this->drawGameSplashScreen(loadedGames[2], 0, 80 + yOffset);
//
//   fxManager->oled->u8g2.sendBuffer();
//
// }

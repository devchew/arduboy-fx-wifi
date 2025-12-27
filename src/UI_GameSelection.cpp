//
// Created by Infin on 13.12.2025.
//

#include "UI_GameSelection.h"

#include "UI.h"


UI_GameSelection::UI_GameSelection(FxManager& fx) {
  fxManager = &fx;
  currentGame = fxManager->gameLibrary->getGameInfo(currentCategoryIndex, currentGameIndex);
  currentCategory = fxManager->gameLibrary->getCategory(currentCategoryIndex);
  gamesInCategory = fxManager->gameLibrary->getGamesCount(currentCategoryIndex);
  categoriesCount = fxManager->gameLibrary->getCategoryCount();
}

UI_GameSelection::~UI_GameSelection() {
  fxManager = nullptr;
  delete &currentGame;
  delete &currentCategory;
}

void UI_GameSelection::drawGameSplashScreen(const GameInfo& game, int8_t x_offset, int8_t y_offset) const {
  fxManager->oled->u8g2.setFont(u8g2_font_profont15_tr);
  uint8_t textWidth = fxManager->oled->u8g2.getStrWidth(game.title.c_str());
  fxManager->oled->u8g2.drawStr(((128 - textWidth) / 2) + x_offset, 30 + y_offset, game.title.c_str());

  fxManager->oled->u8g2.setFont(u8g2_font_4x6_tr);
  textWidth = fxManager->oled->u8g2.getStrWidth(game.author.c_str());
  fxManager->oled->u8g2.drawStr(((128 - textWidth) / 2) + x_offset, 40 + y_offset, game.author.c_str());
}

void UI_GameSelection::drawCategoryScreen(const GameCategory &category, int8_t x_offset, int8_t y_offset) const {
  fxManager->oled->u8g2.setFont(u8g2_font_profont15_tr);
  uint8_t textWidth = fxManager->oled->u8g2.getStrWidth(category.categoryName.c_str());
  fxManager->oled->u8g2.drawStr(((128 - textWidth) / 2) + x_offset, 30 + y_offset, category.categoryName.c_str());

  String gameCountText = String(gamesInCategory) + " games";
  fxManager->oled->u8g2.setFont(u8g2_font_4x6_tr);

  textWidth = fxManager->oled->u8g2.getStrWidth(gameCountText.c_str());
  fxManager->oled->u8g2.drawStr(((128 - textWidth) / 2) + x_offset, 40 + y_offset, gameCountText.c_str());
}

void UI_GameSelection::drawNavbar() const {
  fxManager->oled->u8g2.setFont(u8g2_font_4x6_tr);
  fxManager->oled->u8g2.setFontMode(1);
  fxManager->oled->u8g2.setBitmapMode(1);

  fxManager->oled->u8g2.drawXBMP(3, 57, 3, 5, sprite_left_mini);
  fxManager->oled->u8g2.drawStr(9, 62, "Type");
  fxManager->oled->u8g2.drawXBMP(27, 57, 3, 5, sprite_right_mini);


  fxManager->oled->u8g2.drawXBMP(37, 58, 5, 3, sprite_up_mini);
  fxManager->oled->u8g2.drawStr(45, 62, "Game");
  fxManager->oled->u8g2.drawXBMP(63, 58, 5, 3, sprite_down_mini);

  fxManager->oled->u8g2.drawStr(75, 62, "Back");
  if (!inCategoryScreen) {
    fxManager->oled->u8g2.drawStr(104, 62, "Select");
  }
  fxManager->oled->u8g2.drawXBMP(92, 56, 11, 7, sprite_action_buttons_mini);

}

void UI_GameSelection::draw() {
  if (fxManager->hid->pressed(Buttons::RIGHT) && currentCategoryIndex < categoriesCount - 1) {
    delay(200); // simple debounce
    currentCategoryIndex ++;
    currentGameIndex = 0;
    inCategoryScreen = true;
    currentGame = fxManager->gameLibrary->getGameInfo(currentCategoryIndex, currentGameIndex);
    currentCategory = fxManager->gameLibrary->getCategory(currentCategoryIndex);
    gamesInCategory = fxManager->gameLibrary->getGamesCount(currentCategoryIndex);
  }

  if (fxManager->hid->pressed(Buttons::LEFT) && currentCategoryIndex > 0) {
    delay(200); // simple debounce
    currentCategoryIndex --;
    currentGameIndex = 0;
    inCategoryScreen = true;
    currentGame = fxManager->gameLibrary->getGameInfo(currentCategoryIndex, currentGameIndex);
    currentCategory = fxManager->gameLibrary->getCategory(currentCategoryIndex);
    gamesInCategory = fxManager->gameLibrary->getGamesCount(currentCategoryIndex);
  }

  if (fxManager->hid->pressed(Buttons::DOWN) && currentGameIndex < gamesInCategory - 1) {
    delay(200); // simple debounce
    if (inCategoryScreen) {
      inCategoryScreen = false;
      currentGameIndex = 0;
    } else {
      currentGameIndex ++;
      currentGame = fxManager->gameLibrary->getGameInfo(currentCategoryIndex, currentGameIndex);
    }
  }

  if (fxManager->hid->pressed(Buttons::UP)) {
    delay(200); // simple debounce
    if (currentGameIndex <= 0) {
      inCategoryScreen = true;
    } else {
      currentGameIndex --;
      currentGame = fxManager->gameLibrary->getGameInfo(currentCategoryIndex, currentGameIndex);
    }
  }

  if (fxManager->hid->pressed(Buttons::A) && currentGame.filePath.length() > 0 && currentGameIndex > 0) {
    delay(200); // simple debounce
    fxManager->flashGame(currentGame.filePath.c_str());
  }

  fxManager->oled->u8g2.clearBuffer();

  drawNavbar();

  fxManager->oled->u8g2.setFont(u8g2_font_profont15_tr);

  if (inCategoryScreen) {
    this->drawCategoryScreen(currentCategory);
  } else {
    this->drawGameSplashScreen(currentGame);
  }

  fxManager->oled->u8g2.sendBuffer();

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

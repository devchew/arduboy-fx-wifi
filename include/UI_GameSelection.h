//
// Created by Infin on 13.12.2025.
//

#ifndef ARDUBOY_FX_WIFI_UI_GAMESELECTION_H
#define ARDUBOY_FX_WIFI_UI_GAMESELECTION_H


#include <Arduino.h>
#include <U8g2lib.h>
#include "GameLibrary.h"
#include "FxManager.h"
#include <Sprites.h>


class UI_GameSelection {
  private:
    FxManager* fxManager = nullptr;

    // 0 = no direction, 1 = up, 2 = down, 3 = left, 4 = right
    uint8_t direction = 0;
    int yOffset = 0;
    int xOffset = 0;
    // one game before, current game, one game after index


    uint8_t currentCategoryIndex = 0;
    uint8_t categoriesCount = 0;
    uint8_t currentGameIndex = 0;
    uint8_t gamesInCategory = 0;
    bool inCategoryScreen = true;
    GameInfo currentGame = GameInfo();
    GameCategory currentCategory = GameCategory();

    void drawGameSplashScreen(const GameInfo& game, int8_t x_offset = 0, int8_t y_offset = 0) const;
    void drawCategoryScreen(const GameCategory& category, int8_t x_offset = 0, int8_t y_offset = 0) const;


  public:
    UI_GameSelection(FxManager& fx);
    ~UI_GameSelection();

    void draw();
};

#endif //ARDUBOY_FX_WIFI_UI_GAMESELECTION_H
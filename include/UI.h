#ifndef ARDUBOY_FX_WIFI_UI_H
#define ARDUBOY_FX_WIFI_UI_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "HID.h"
#include "Games.h"
#include "FxManager.h"
#include "Splash.h"
#include "Buttons_Sprites.h"
#include "Icons.h"
#include <deque>

enum class Screen {
  SPLASH,
  BUTTONS_TEST,
  GAME_LIST,
  FLASH_GAME,
};

class UI {
  private:
    U8G2_SCREEN* u8g2 = nullptr;
    HID* hid = nullptr;
    FxManager* fxManager = nullptr;
    uint8_t buttonsState = -1;
    Screen currentScreen = Screen::SPLASH;

    // game list variables
    // 0 = no direction, 1 = up, 2 = down, 3 = left, 4 = right
    uint8_t direction = 0;
    int yOffset = 0;
    int xOffset = 0;
    // one game before, current game, one game after index
    std::array<GameInfo, GAMES_PER_PAGE> loadedGames = {
      GameInfo(), GameInfo(), GameInfo()
    };
    std::array<GamesCategory, MAX_CATEGORIES> categories = {};
    uint8_t currentCategoryIndex = 0;
    uint8_t currentGameIndex = 0;


    void drawTextCenter(const char* text, int8_t x_offset = 0, int8_t y_offset = 0) const;
    void drawGameSplashScreen(const GameInfo& game, int8_t x_offset = 0, int8_t y_offset = 0) const;
    void drawCategoryScreen(const GamesCategory& category, int8_t x_offset = 0, int8_t y_offset = 0) const;

    void screenButtonsTest() const;
    void splashScreen() ;
    void screenGameList() ;
    void screenFlashGame() const ;

  public:
    UI();
    ~UI();
    bool begin(U8G2_SCREEN& u8g2, HID& hid, FxManager& fxManager);
    void update();
};

#endif //ARDUBOY_FX_WIFI_UI_H
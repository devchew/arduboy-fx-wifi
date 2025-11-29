//
// Created by Infin on 29.11.2025.
//

#ifndef ARDUBOY_FX_WIFI_GAMES_H
#define ARDUBOY_FX_WIFI_GAMES_H

#include <Arduino.h>

#define MAX_CATEGORIES 20
#define GAMES_PER_PAGE 5

struct GameInfo {
  String filePath;
  String title;
  String date;
  String author;
  String description;
  String license;
};

struct GamesCategory {
  String categoryName;
  String categoryPath;
  uint8_t gameCount;
};

#endif //ARDUBOY_FX_WIFI_GAMES_H
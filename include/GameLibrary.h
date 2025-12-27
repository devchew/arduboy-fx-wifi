
#ifndef ARDUBOY_FX_WIFI_GAMELIBRARY_H
#define ARDUBOY_FX_WIFI_GAMELIBRARY_H

#include <Arduino.h>
#include <MacroLogger.h>
#include "FileSystemManager.h"
#include <vector>

struct GameInfo {
  String filePath;
  String title;
  String date;
  String author;
  String description;
  String license;
};

struct GameCategory {
  String categoryName;
  String categoryPath;
};

struct Games : GameCategory {
  std::vector<GameInfo> games;
};

class GameLibrary {
private:
  std::vector<Games> games = {};
  FileSystemManager* fileSystemManager = nullptr;

  GameInfo findGameInFolder(const File& folder) const;
  void extractCategoryMetadata(const File &folder, GameCategory &outCategory);

public:
  GameLibrary();
  ~GameLibrary();

  // initialization
  void begin(FileSystemManager& fs);
  void end();

  // load games from filesystem
  void loadGames(const String& rootPath);

  // get category
  GameCategory getCategory(const uint8_t index) const;
  uint8_t getCategoryCount() const;

  // get games in category
  uint8_t getGamesCount(const uint8_t category_index) const;
  GameInfo getGameInfo(const uint8_t category_index, const uint8_t game_index) const;

};

#endif //ARDUBOY_FX_WIFI_GAMELIBRARY_H
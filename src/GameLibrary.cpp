#include "GameLibrary.h"

GameLibrary::GameLibrary() {}
GameLibrary::~GameLibrary() {
  end();
}

void GameLibrary::begin(FileSystemManager& fs) {
  this->fileSystemManager = &fs;
}

void GameLibrary::end() {
  games.clear();
  this->fileSystemManager = nullptr;
}

// find game in the given folder
GameInfo GameLibrary::findGameInFolder(const File &folder) const {

  // Found the game path, it should be a first .hex file inside this directory
  File gameDir = this->fileSystemManager->openFile(folder.path());
  if (!gameDir || !gameDir.isDirectory()) {
    Logger::error("Game directory not found: %s\n", folder.name());
    return GameInfo{ "", "Error", "", "", "", ""  };
  }
  File gameFile = gameDir.openNextFile();
  String title = String(folder.name());
  while (gameFile) {
    String filename = String(gameFile.name());
    if (filename.endsWith(".hex")) {
      // this is the hex file, exit the loop
      break;
    }
    gameFile = gameFile.openNextFile();
  }

  return GameInfo{
    gameFile ? String(gameFile.path()) : "",
    title,
    "",
    gameFile ? String(gameFile.name()) : "",
    "",
    ""
   };

}

void GameLibrary::extractCategoryMetadata(const File &folder, GameCategory &outCategory) {
  // Placeholder implementation
  outCategory.categoryName = String(folder.name());
  outCategory.categoryPath = String(folder.name());
}

void GameLibrary::loadGames(const String& rootPath) {
  if (!fileSystemManager || !fileSystemManager->isInitialized()) {
    Logger::error("FileSystemManager not initialized");
    return;
  }

  games.clear();

  File gamesDir = fileSystemManager->openFile(rootPath);
  if (!gamesDir || !gamesDir.isDirectory()) {
    Logger::error("Games directory not found: %s\n", rootPath.c_str());
    return;
  }

  File entry = gamesDir.openNextFile();
  while (entry) {
    if (!entry.isDirectory()) {
      // is not a directory, skip
      entry = gamesDir.openNextFile();
      continue;
    }

    Games category;

    // find games in the category folder
    std::vector<GameInfo> gamesInCategory = {};

    // loop through files in the category folder
    File gameFile = entry.openNextFile();
    while (gameFile) {
      if (!gameFile.isDirectory()) {
        // skip non-directory files
        gameFile = entry.openNextFile();
        continue;
      }
      GameInfo game = findGameInFolder(gameFile);
      if (game.filePath.length() > 0) {
        gamesInCategory.push_back(game);
      }
      gameFile = entry.openNextFile();
    }

    extractCategoryMetadata(entry, category);
    category.games = gamesInCategory;

    games.push_back(category);

    entry = gamesDir.openNextFile();
  }
  gamesDir.close();
}

GameCategory GameLibrary::getCategory(const uint8_t index) const {
  if (index >= games.size()) {
    return GameCategory{ "", "" };
  }
  return static_cast<GameCategory>(games.at(index));
}

uint8_t GameLibrary::getCategoryCount() const {
  return games.size();
}

uint8_t GameLibrary::getGamesCount(const uint8_t category_index) const {
  if (category_index < games.size()) {
    return games.at(category_index).games.size();
  }
  return 0;
}

GameInfo GameLibrary::getGameInfo(const uint8_t category_index, const uint8_t game_index) const {
  if (category_index > games.size()) {
    return GameInfo{ "", "Unknown categorry", "", "", "", "" };
  }
  const Games& category = games.at(category_index);
  if (game_index >= category.games.size()) {
    return GameInfo{ "", "Unknown Game", "", "", "", "" };
  }
  return category.games.at(game_index);
}
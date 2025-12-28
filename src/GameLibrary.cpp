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

  String filename = String(gameFile.path());

  gameFile.close();

  return GameInfo{
    filename,
    title,
    "",
    "",
    "",
    ""
   };

}

void GameLibrary::extractCategoryMetadata(const File &folder, GameCategory &outCategory) {
  // Placeholder implementation
  outCategory.categoryName = String(folder.name());
  outCategory.categoryPath = String(folder.name());
}

void GameLibrary::loadLibraryFromFolder() {
  if (!fileSystemManager || !fileSystemManager->isInitialized()) {
    Logger::error("FileSystemManager not initialized");
    return;
  }

  games.clear();

  File gamesDir = fileSystemManager->openFile(GAME_LIBRARY_PATH);
  if (!gamesDir || !gamesDir.isDirectory()) {
    Logger::error("Games directory not found: %s\n", GAME_LIBRARY_PATH);
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

    gameFile.close();

    extractCategoryMetadata(entry, category);
    category.games = gamesInCategory;

    games.push_back(category);

    entry = gamesDir.openNextFile();
  }
  gamesDir.close();
}

void GameLibrary::loadGames() {
  TaskHandle_t gamesLoaderTask = nullptr;
  xTaskCreatePinnedToCore(
    [](void* param) {
      GameLibrary* library = static_cast<GameLibrary*>(param);
      if (!library) {
        Logger::error("GameLibrary pointer is null in LoadGamesTask");
        vTaskDelete(nullptr);
        return;
      }

      library->loading = true;
      library->loaded = false;

      Logger::info("Loading game library from filesystem...");
      library->loadLibraryFromFolder();
      Logger::info("Game library loaded: %u categories\n", library->getCategoryCount());

      library->loading = false;
      library->loaded = true;

      vTaskDelete(nullptr);
    },
    "LoadGamesTask",
    8192,
    this,
    1,
    &gamesLoaderTask,
    1
  );
}

GameCategory GameLibrary::getCategory(uint8_t index) const {
  if (index >= games.size()) {
    return GameCategory{ "", "" };
  }
  return static_cast<GameCategory>(games.at(index));
}

uint8_t GameLibrary::getCategoryCount() const {
  return games.size();
}

uint8_t GameLibrary::getGamesCount(uint8_t category_index) const {
  if (category_index < games.size()) {
    return games.at(category_index).games.size();
  }
  return 0;
}

GameInfo GameLibrary::getGameInfo(uint8_t category_index, uint8_t game_index) const {
  if (category_index >= games.size()) {
    return GameInfo{ "", "Unknown category", "", "", "", "" };
  }
  const Games& category = games.at(category_index);
  if (game_index >= category.games.size()) {
    return GameInfo{ "", "Unknown Game", "", "", "", "" };
  }
  return category.games.at(game_index);
}
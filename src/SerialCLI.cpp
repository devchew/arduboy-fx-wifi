#include "SerialCLI.h"

SerialCLI::SerialCLI(FxManager* fxManager) {
  this->fxManager = fxManager;

  Serial.println();
  Serial.println("=====================================");
  Serial.println("    ARDUBOY FX WIFI PROGRAMMER");
  Serial.println("=====================================");
}

SerialCLI::~SerialCLI() { fxManager = nullptr; }

void SerialCLI::update() {
  if (Serial.available()) {
    // Read out string from the serial monitor
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() == 0) {
      Serial.println("Please enter a command");
      return;
    }

    // Parse command and arguments
    int spaceIndex = input.indexOf(' ');
    String command =
        (spaceIndex == -1) ? input : input.substring(0, spaceIndex);
    String args = (spaceIndex == -1) ? "" : input.substring(spaceIndex + 1);

    command.toLowerCase();
    args.trim();

    if (command == "mode") {
      if (args.length() == 0) {
        Serial.println("Usage: mode <game|master|programming>");
        return;
      }

      args.toLowerCase();
      if (args == "game" || args == "g") {
        fxManager->setMode(FxMode::GAME);
        return;
      }
      if (args == "master" || args == "m") {
        fxManager->setMode(FxMode::MASTER);
        return;
      }
      if (args == "programming" || args == "prog" || args == "p") {
        fxManager->setMode(FxMode::PROGRAMMING);
        return;
      }
      Serial.println(
          "Invalid mode. Available modes: game, master, programming");

      return;
    }

    if (command == "flash") {
      if (args.length() == 0) {
        Serial.println("Usage: flash <filename>");
        return;
      }
      fxManager->flashGame(args);
      return;
    }

    if (command == "ls") {
      if (args.length() == 0) {
        Serial.println("Usage: ls <path>");
        return;
      }
      fxManager->fileSystem->listDirectory(args);
      return;
    }

    if (command== "lg") {
      fxManager->gameLibrary->loadGames("/arduboy");
      return;
    }

    if (command == "categories") {
      int categoryCount = fxManager->gameLibrary->getCategoryCount();
      Serial.println("Game Categories:");
      for (int i = 0; i < categoryCount; i++) {
        GameCategory category = fxManager->gameLibrary->getCategory(i);
        Serial.println(String(i) + ": " + category.categoryName + " (" +
                       String(fxManager->gameLibrary->getGamesCount(i)) + " games)");
      }
      return;
    }

    if (command == "games") {
      if (args.length() == 0) {
        Serial.println("Usage: games <category index>");
        return;
      }
      int categoryIndex = args.toInt();
      GameCategory category = fxManager->gameLibrary->getCategory(categoryIndex);
      if (category.categoryName.length() == 0) {
        Serial.println("Invalid category index");
        return;
      }
      int gameCount = fxManager->gameLibrary->getGamesCount(categoryIndex);
      Serial.println("Games in Category: " + category.categoryName);
      for (int i = 0; i < gameCount; i++) {
        GameInfo game = fxManager->gameLibrary->getGameInfo(categoryIndex, i);
        Serial.println(String(i) + ": " + game.title + " by " + game.author);
      }
      return;
    }

    if (command == "game") {
      if (args.length() == 0) {
        Serial.println("Usage: game <category index> <game index>");
        return;
      }
      int spaceIdx = args.indexOf(' ');
      if (spaceIdx == -1) {
        Serial.println("Usage: game <category index> <game index>");
        return;
      }
      String categoryStr = args.substring(0, spaceIdx);
      String gameStr = args.substring(spaceIdx + 1);
      int categoryIndex = categoryStr.toInt();
      int gameIndex = gameStr.toInt();
      GameCategory category = fxManager->gameLibrary->getCategory(categoryIndex);
      if (category.categoryName.length() == 0) {
        Serial.println("Invalid category index");
        return;
      }
      GameInfo game = fxManager->gameLibrary->getGameInfo(categoryIndex, gameIndex);
      if (game.filePath.length() == 0) {
        Serial.println("Invalid game index");
        return;
      }
      // print game info
      Serial.println("Game Info:");
      Serial.println("Title: " + game.title);
      Serial.println("Author: " + game.author);
      Serial.println("Date: " + game.date);
      Serial.println("Description: " + game.description);
      Serial.println("License: " + game.license);
      return;
    }

    if (command == "card") {
      fxManager->fileSystem->getInfo();
      return;
    }

    if (command == "mkdir") {
          if (args.length() == 0) {
            Serial.println("Usage: mkdir <path>");
            return;
          }
          fxManager->fileSystem->createDirectory(args);
          return;
        }



        if (command == "cat") {
          if (args.length() == 0) {
            Serial.println("Usage: cat <filename>");
            return;
          }
          fxManager->fileSystem->readFile(args);
          return;
        }

        if (command == "reset") {
          fxManager->reset();
          return;
        }

        if (command == "info") {
          fxManager->printInfo();
          return;
        }


    if (command == "oled") {
      if (args.length() == 0) {
        Serial.println("Usage: oled <reset|master|slave|disable|enable>");
        return;
      }

      args.toLowerCase();
      if (args == "reset") {
        fxManager->oled->reset();
        return;
      }
      if (args == "master") {
        fxManager->oled->master();
        return;
      }
      if (args == "slave") {
        fxManager->oled->slave();
        return;
      }
      if (args == "enable") {
        fxManager->oled->enable();
        return;
      }
      if (args == "disable") {
        fxManager->oled->disable();
        return;
      }
      Serial.println("Invalid OLED command. Available commands: reset, master, slave");

      return;
    }

    if (command == "reset") {
      fxManager->reset();
      return;
    }

    if (command == "info") {
      fxManager->printInfo();
      return;
    }
  }
}
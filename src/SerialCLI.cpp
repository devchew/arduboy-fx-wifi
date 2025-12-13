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

    if (command== "games") {
      auto categories = fxManager->getCategories();
      if (categories[0].categoryName.length() == 0) {
        Serial.println("No game categories found.");
        return;
      }
      for ( const auto& category : categories) {
        if (category.categoryName.length() == 0) {
          break;
        }
        Serial.printf("Category: %s (%u games)\n", category.categoryName.c_str(), category.gameCount);
      }

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
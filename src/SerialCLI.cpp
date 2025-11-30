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
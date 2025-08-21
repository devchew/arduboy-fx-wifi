#ifndef SERIALCLI_H
#define SERIALCLI_H

#include <Arduino.h>

class ArduboyController;
class FileSystemManager;

class SerialCLI {
private:
  ArduboyController* arduboy;
  FileSystemManager* fileSystem;
  bool initialized;

  // Command handlers
  void handleHelp();
  void handleInfo();
  void handleFlash(const String& filename);
  void handleCheck();
  void handleReset();
  void handlePowerOn();
  void handlePowerOff();
  void handleList(const String& path = "/");
  void handleRead(const String& filename);
  void handleWrite(const String& filename, const String& content);
  void handleDelete(const String& filename);
  void handleFormat();

  // Utility functions
  void printWelcome();
  void printPrompt();
  void printError(const String& message);
  void printSuccess(const String& message);

public:
  SerialCLI();
  ~SerialCLI();

  bool begin(ArduboyController* arduboyController, FileSystemManager* fsManager);
  void end();
  void processInput();
  void update();
};

#endif // SERIALCLI_H

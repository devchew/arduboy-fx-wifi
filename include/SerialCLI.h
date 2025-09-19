#ifndef SERIALCLI_H
#define SERIALCLI_H

#include <Arduino.h>
#include <SimpleCLI.h>

class ArduboyController;
class FileSystemManager;
class OLEDController;

class SerialCLI {
private:
  ArduboyController* arduboy;
  FileSystemManager* fileSystem;
  OLEDController* oled;
  bool initialized;
  SimpleCLI cli;

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

  // OLED control handlers
  void handleOLEDReset();
  void handleOLEDMaster();
  void handleOLEDSlave();
  void handleOLEDHello();

  // Utility functions
  void printWelcome();
  void printPrompt();
  void printError(const String& message);
  void printSuccess(const String& message);

public:
  SerialCLI();
  ~SerialCLI();

  bool begin(ArduboyController* arduboyController, FileSystemManager* fsManager,
             OLEDController* oledController);
  void end();
  void processInput();
  void update();
};

#endif // SERIALCLI_H

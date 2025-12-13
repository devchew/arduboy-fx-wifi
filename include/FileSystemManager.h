#ifndef FILESYSTEMMANAGER_H
#define FILESYSTEMMANAGER_H

#include <Arduino.h>
#include "SD.h"
#include <SPI.h>
#include "config.h"

class FileSystemManager {
private:
  bool initialized = false;
  SPIClass* sdSPI = nullptr;
  void printFileInfo(File file, int level = 0);

public:
  FileSystemManager();
  ~FileSystemManager();

  // Initialization
  bool begin();
  void end();
  bool isInitialized() const { return initialized; }

  // Directory operations
  void listDirectory(const String& path = "/", uint8_t maxLevels = 3);
  bool createDirectory(const String& path);
  bool removeDirectory(const String& path);
  bool directoryExists(const String& path);

  // File operations
  bool fileExists(const String& path);
  size_t getFileSize(const String& path);
  String readFile(const String& path);
  bool writeFile(const String& path, const String& content);
  bool appendFile(const String& path, const String& content);
  bool deleteFile(const String& path);
  bool copyFile(const String& sourcePath, const String& destPath);

  // System operations
  void getInfo();

  // File streaming (for large files)
  File openFile(const String& path, const String& mode = "r");
  void closeFile(File& file);

  // Hex file specific operations
  bool isValidHexFile(const String& path);
  void listHexFiles();
};

#endif // FILESYSTEMMANAGER_H

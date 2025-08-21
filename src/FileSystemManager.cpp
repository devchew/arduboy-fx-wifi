#include "FileSystemManager.h"

FileSystemManager::FileSystemManager() : initialized(false) {}

FileSystemManager::~FileSystemManager() {
  end();
}

bool FileSystemManager::begin() {
  if (initialized) {
    return true;
  }

  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS filesystem");
    return false;
  }

  initialized = true;
  Serial.println("FileSystem initialized successfully");
  return true;
}

void FileSystemManager::end() {
  if (initialized) {
    LittleFS.end();
    initialized = false;
  }
}

// ==========================================
// DIRECTORY OPERATIONS
// ==========================================

void FileSystemManager::listDirectory(const String& path, uint8_t maxLevels) {
  if (!initialized) {
    Serial.println("FileSystem not initialized");
    return;
  }

  File root = LittleFS.open(path);
  if (!root) {
    Serial.printf("Failed to open directory: %s\n", path.c_str());
    return;
  }

  if (!root.isDirectory()) {
    Serial.printf("Path is not a directory: %s\n", path.c_str());
    root.close();
    return;
  }

  Serial.printf("Listing directory: %s\n", path.c_str());
  Serial.println("=====================================");

  File file = root.openNextFile();
  while (file) {
    printFileInfo(file, 0);
    if (file.isDirectory() && maxLevels > 0) {
      // Recursively list subdirectories
      String subPath = String(file.path());
      File subRoot = LittleFS.open(subPath);
      if (subRoot) {
        File subFile = subRoot.openNextFile();
        while (subFile) {
          printFileInfo(subFile, 1);
          subFile.close();
          subFile = subRoot.openNextFile();
        }
        subRoot.close();
      }
    }
    file.close();
    file = root.openNextFile();
  }
  root.close();
  Serial.println("=====================================");
}

bool FileSystemManager::createDirectory(const String& path) {
  if (!initialized) {
    return false;
  }
  return LittleFS.mkdir(path);
}

bool FileSystemManager::removeDirectory(const String& path) {
  if (!initialized) {
    return false;
  }
  return LittleFS.rmdir(path);
}

bool FileSystemManager::directoryExists(const String& path) {
  if (!initialized) {
    return false;
  }
  File dir = LittleFS.open(path);
  if (dir && dir.isDirectory()) {
    dir.close();
    return true;
  }
  if (dir) dir.close();
  return false;
}

// ==========================================
// FILE OPERATIONS
// ==========================================

bool FileSystemManager::fileExists(const String& path) {
  if (!initialized) {
    return false;
  }
  return LittleFS.exists(path);
}

size_t FileSystemManager::getFileSize(const String& path) {
  if (!initialized || !fileExists(path)) {
    return 0;
  }
  File file = LittleFS.open(path, "r");
  if (!file) {
    return 0;
  }
  size_t size = file.size();
  file.close();
  return size;
}

String FileSystemManager::readFile(const String& path) {
  if (!initialized) {
    return "";
  }

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.printf("Failed to open file for reading: %s\n", path.c_str());
    return "";
  }

  String content = file.readString();
  file.close();
  return content;
}

bool FileSystemManager::writeFile(const String& path, const String& content) {
  if (!initialized) {
    return false;
  }

  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.printf("Failed to open file for writing: %s\n", path.c_str());
    return false;
  }

  size_t bytesWritten = file.print(content);
  file.close();
  return bytesWritten == content.length();
}

bool FileSystemManager::appendFile(const String& path, const String& content) {
  if (!initialized) {
    return false;
  }

  File file = LittleFS.open(path, "a");
  if (!file) {
    Serial.printf("Failed to open file for appending: %s\n", path.c_str());
    return false;
  }

  size_t bytesWritten = file.print(content);
  file.close();
  return bytesWritten == content.length();
}

bool FileSystemManager::deleteFile(const String& path) {
  if (!initialized) {
    return false;
  }
  return LittleFS.remove(path);
}

bool FileSystemManager::copyFile(const String& sourcePath, const String& destPath) {
  if (!initialized || !fileExists(sourcePath)) {
    return false;
  }

  String content = readFile(sourcePath);
  if (content.length() == 0 && getFileSize(sourcePath) > 0) {
    return false; // Read failed
  }

  return writeFile(destPath, content);
}

// ==========================================
// SYSTEM OPERATIONS
// ==========================================

bool FileSystemManager::format() {
  if (!initialized) {
    return false;
  }
  
  Serial.println("Formatting filesystem...");
  LittleFS.end();
  initialized = false;
  
  bool success = LittleFS.format();
  if (success) {
    Serial.println("Format successful");
    return begin(); // Reinitialize
  } else {
    Serial.println("Format failed");
    return false;
  }
}

void FileSystemManager::getInfo() {
  if (!initialized) {
    Serial.println("FileSystem not initialized");
    return;
  }

  size_t total = getTotalBytes();
  size_t used = getUsedBytes();
  size_t free = getFreeBytes();

  Serial.println("=====================================");
  Serial.println("FILESYSTEM INFO");
  Serial.println("=====================================");
  Serial.printf("Total space: %u bytes (%.2f KB)\n", total, total / 1024.0);
  Serial.printf("Used space:  %u bytes (%.2f KB)\n", used, used / 1024.0);
  Serial.printf("Free space:  %u bytes (%.2f KB)\n", free, free / 1024.0);
  Serial.printf("Usage: %.1f%%\n", (used * 100.0) / total);
  Serial.println("=====================================");
}

size_t FileSystemManager::getTotalBytes() {
  return LittleFS.totalBytes();
}

size_t FileSystemManager::getUsedBytes() {
  return LittleFS.usedBytes();
}

size_t FileSystemManager::getFreeBytes() {
  return getTotalBytes() - getUsedBytes();
}

// ==========================================
// FILE STREAMING
// ==========================================

File FileSystemManager::openFile(const String& path, const String& mode) {
  if (!initialized) {
    return File();
  }
  return LittleFS.open(path, mode.c_str());
}

void FileSystemManager::closeFile(File& file) {
  if (file) {
    file.close();
  }
}

// ==========================================
// HEX FILE OPERATIONS
// ==========================================

bool FileSystemManager::isValidHexFile(const String& path) {
  if (!fileExists(path)) {
    return false;
  }

  // Check file extension
  if (!path.endsWith(".hex")) {
    return false;
  }

  // Read first few lines to validate Intel HEX format
  File file = openFile(path, "r");
  if (!file) {
    return false;
  }

  String line = file.readStringUntil('\n');
  file.close();

  // Intel HEX lines start with ':'
  return line.startsWith(":");
}

void FileSystemManager::listHexFiles() {
  if (!initialized) {
    Serial.println("FileSystem not initialized");
    return;
  }

  Serial.println("Available HEX files:");
  Serial.println("=====================================");

  File root = LittleFS.open("/");
  if (!root) {
    Serial.println("Failed to open root directory");
    return;
  }

  bool foundHex = false;
  File file = root.openNextFile();
  while (file) {
    String filename = String(file.name());
    if (filename.endsWith(".hex")) {
      foundHex = true;
      Serial.printf("  %s (%u bytes)\n", filename.c_str(), file.size());
    }
    file.close();
    file = root.openNextFile();
  }
  root.close();

  if (!foundHex) {
    Serial.println("  No HEX files found");
  }
  Serial.println("=====================================");
}

// ==========================================
// UTILITY FUNCTIONS
// ==========================================

void FileSystemManager::printFileInfo(File file, int level) {
  // Indentation for subdirectories
  for (int i = 0; i < level; i++) {
    Serial.print("  ");
  }

  if (file.isDirectory()) {
    Serial.printf("[DIR]  %s/\n", file.name());
  } else {
    Serial.printf("[FILE] %s (%u bytes)\n", file.name(), file.size());
  }
}

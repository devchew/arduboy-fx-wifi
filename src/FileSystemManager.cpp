#include "FileSystemManager.h"

FileSystemManager::FileSystemManager() {}

FileSystemManager::~FileSystemManager() {
  end();
}

bool FileSystemManager::begin() {
  if (initialized) {
    return true;
  }

  // SDCARD
  pinMode(SD_CS_PIN, OUTPUT); // SS

  sdSPI = new SPIClass(2);
  sdSPI->begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  if (!SD.begin(SD_CS_PIN, *sdSPI)) {
    Logger::error("Nie udalo sie zainicjowac karty SD");
    return false;
  }

  if (SD.cardType() == CARD_NONE) {
    Logger::error("Brak karty SD");
    return false;
  }

  initialized = true;
  Logger::info("FileSystem initialized successfully");
  return true;
}

void FileSystemManager::end() {
  if (initialized) {
    SD.end();
    initialized = false;
  }
}

// ==========================================
// DIRECTORY OPERATIONS
// ==========================================

void FileSystemManager::listDirectory(const String& path, uint8_t maxLevels) {
  if (!initialized) {
    Logger::error("FileSystem not initialized");
    return;
  }

  File root = SD.open(path);
  if (!root) {
    Logger::error("Failed to open directory: %s\n", path.c_str());
    return;
  }

  if (!root.isDirectory()) {
    Logger::info("Path is not a directory: %s\n", path.c_str());
    root.close();
    return;
  }

  Logger::info("Listing directory: %s\n", path.c_str());
  Logger::info("=====================================");

  File file = root.openNextFile();
  while (file) {
    printFileInfo(file, 0);
    if (file.isDirectory() && maxLevels > 0) {
      // Build proper subPath: if file.name() is absolute use it, otherwise append to current path
      String childName = String(file.name());
      String subPath;
      if (childName.length() > 0 && childName.charAt(0) == '/') {
        subPath = childName;
      } else {
        subPath = path;
        if (!subPath.endsWith("/")) subPath += "/";
        subPath += childName;
      }

      File subRoot = SD.open(subPath);
      if (subRoot) {
        File subFile = subRoot.openNextFile();
        while (subFile) {
          printFileInfo(subFile, 1);
          subFile.close();
          subFile = subRoot.openNextFile();
        }
        subRoot.close();
      } else {
        Logger::info("Failed to open subdirectory: %s\n", subPath.c_str());
      }
    }
    file.close();
    file = root.openNextFile();
  }
  root.close();
  Logger::info("=====================================");
}

bool FileSystemManager::createDirectory(const String& path) {
  if (!initialized) {
    return false;
  }
  return SD.mkdir(path);
}

bool FileSystemManager::removeDirectory(const String& path) {
  if (!initialized) {
    return false;
  }
  return SD.rmdir(path);
}

bool FileSystemManager::directoryExists(const String& path) {
  if (!initialized) {
    return false;
  }
  File dir = SD.open(path);
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
  return SD.exists(path);
}

size_t FileSystemManager::getFileSize(const String& path) {
  if (!initialized || !fileExists(path)) {
    return 0;
  }
  File file = openFile(path, "r");
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

  File file = openFile(path, "r");
  if (!file) {
    Logger::info("Failed to open file for reading: %s\n", path.c_str());
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

  // Overwrite: remove existing then open for write
  if (SD.exists(path)) {
    Logger::info("File exists: %s\n", path.c_str());
    return false;
  }

  File file = openFile(path, "w");
  if (!file) {
    Logger::info("Failed to open file for writing: %s\n", path.c_str());
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

  File file = openFile(path, "a");
  if (!file) {
    Logger::info("Failed to open file for appending: %s\n", path.c_str());
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
  return SD.remove(path);
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

void FileSystemManager::getInfo() {
  if (!initialized) {
    Logger::error("FileSystem not initialized");
    return;
  }

  auto cardType = SD.cardType();
  Logger::info("=====================================");
  Logger::info("FILESYSTEM INFO");
  Logger::info("=====================================");
  Logger::info("Card Type: ");
  switch (cardType) {
    case CARD_MMC:
      Logger::info("MMC");
      break;
    case CARD_SD:
      Logger::info("SDSC");
      break;
    case CARD_SDHC:
      Logger::info("SDHC");
      break;
    case CARD_UNKNOWN:
    default:
      Logger::info("UNKNOWN");
      break;
  }

  uint32_t cardSizeMB = (uint32_t)(SD.cardSize() / (1024ULL * 1024ULL));
  Logger::info("Card Size: %u MB\n", cardSizeMB);
  Logger::info("=====================================");
}

// ==========================================
// FILE STREAMING
// ==========================================

File FileSystemManager::openFile(const String& path, const String& mode) {
  if (!initialized) {
    return File();
  }

  if (mode == "r") {
    return SD.open(path, FILE_READ);
  } else if (mode == "w") {
    // overwrite
    if (SD.exists(path)) SD.remove(path);
    return SD.open(path, FILE_WRITE);
  } else if (mode == "a") {
    File f = SD.open(path, FILE_WRITE);
    if (f) f.seek(f.size());
    return f;
  } else {
    // fallback
    return SD.open(path);
  }
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
    Logger::error("FileSystem not initialized");
    return;
  }

  Logger::info("Available HEX files:");
  Logger::info("=====================================");

  File root = SD.open("/");
  if (!root) {
    Logger::error("Failed to open root directory");
    return;
  }

  bool foundHex = false;
  File file = root.openNextFile();
  while (file) {
    String filename = String(file.name());
    if (filename.endsWith(".hex")) {
      foundHex = true;
      Logger::info("  %s (%u bytes)\n", filename.c_str(), file.size());
    }
    file.close();
    file = root.openNextFile();
  }
  root.close();

  if (!foundHex) {
    Logger::info("  No HEX files found");
  }
  Logger::info("=====================================");
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
    Logger::info("[DIR]  %s/\n", file.name());
  } else {
    Logger::info("[FILE] %s (%u bytes)\n", file.name(), file.size());
  }
}

#include "ISPProgrammer.h"

#include <SPI.h>

ISPProgrammer::ISPProgrammer(uint8_t reset_pin) {
  this->reset_pin = reset_pin;
  device_detected = false;

  // Initialize current_device properly
  current_device.signature[0] = 0;
  current_device.signature[1] = 0;
  current_device.signature[2] = 0;
  current_device.name = "";
  current_device.page_size = 0;
  current_device.flash_size = 0;
  current_device.uses_word_addressing = false;
}

ISPProgrammer::~ISPProgrammer() { end(); }

bool ISPProgrammer::begin() {
  // Setup pins
  pinMode(reset_pin, OUTPUT);

  // Initialize SPI
  SPI.begin();
  SPI.setFrequency(100000);  // 100kHz for programming
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  // Reset target
  digitalWrite(reset_pin, HIGH);
  delay(50);
  digitalWrite(reset_pin, LOW);
  delay(50);

  return true;
}

void ISPProgrammer::end() {
  SPI.end();
  digitalWrite(reset_pin, HIGH);  // Release reset
  device_detected = false;
}

bool ISPProgrammer::enterProgrammingMode() {
  // Ensure target is in reset
  digitalWrite(reset_pin, LOW);
  delay(50);

  // Try to sync with target - send programming enable command
  for (int attempts = 0; attempts < 32; attempts++) {
    SPI.transfer(0xAC);
    SPI.transfer(0x53);
    uint8_t response = SPI.transfer(0x00);
    SPI.transfer(0x00);

    if (response == 0x53) {
      Serial.println("Programming mode enabled");
      return detectDevice();
    }

    // Pulse SCK and try again
    digitalWrite(SCK, HIGH);
    delayMicroseconds(10);
    digitalWrite(SCK, LOW);
    delayMicroseconds(10);
  }

  Serial.println("Failed to enable programming mode");
  return false;
}

bool ISPProgrammer::exitProgrammingMode() {
  digitalWrite(reset_pin, HIGH);  // Release reset
  delay(10);
  return true;
}

uint8_t ISPProgrammer::spiTransaction(uint8_t a, uint8_t b, uint8_t c,
                                      uint8_t d) {
  SPI.transfer(a);
  SPI.transfer(b);
  SPI.transfer(c);
  return SPI.transfer(d);
}

bool ISPProgrammer::detectDevice() {
  // Read device signature
  uint8_t sig[3];
  sig[0] = spiTransaction(0x30, 0x00, 0x00, 0x00);
  sig[1] = spiTransaction(0x30, 0x00, 0x01, 0x00);
  sig[2] = spiTransaction(0x30, 0x00, 0x02, 0x00);

  memcpy(current_device.signature, sig, 3);

  // Identify device
  if (sig[0] == ATMEGA32U4_SIGNATURE_0 && sig[1] == ATMEGA32U4_SIGNATURE_1 &&
      sig[2] == ATMEGA32U4_SIGNATURE_2) {
    current_device = getATmega32U4Info();
    device_detected = true;
  } else if (sig[0] == ATMEGA328P_SIGNATURE_0 &&
             sig[1] == ATMEGA328P_SIGNATURE_1 &&
             sig[2] == ATMEGA328P_SIGNATURE_2) {
    current_device = getATmega328PInfo();
    device_detected = true;
  } else {
    current_device.name = "Unknown";
    current_device.page_size = 128;              // Default
    current_device.flash_size = 32768;           // Default
    current_device.uses_word_addressing = true;  // Default
    device_detected = false;
    Serial.printf("Warning: Unknown device signature: 0x%02X 0x%02X 0x%02X\n",
                  sig[0], sig[1], sig[2]);
  }

  return device_detected;
}

bool ISPProgrammer::eraseChip() {
  if (!device_detected) return false;

  Serial.println("Erasing chip...");

  // Send chip erase command
  spiTransaction(0xAC, 0x80, 0x00, 0x00);

  // Wait for erase to complete
  delay(100);

  return true;
}

bool ISPProgrammer::programFlash(const uint8_t* data, uint32_t size) {
  if (!device_detected || !data) return false;

  Serial.println("Programming flash...");

  uint32_t page_size = current_device.page_size;
  uint32_t pages = (size + page_size - 1) / page_size;

  for (uint32_t page = 0; page < pages; page++) {
    uint32_t addr = page * page_size;
    const uint8_t* page_data = &data[addr];

    // Check if page contains data (not all 0xFF)
    bool has_data = false;
    for (uint32_t i = 0; i < page_size && (addr + i) < size; i++) {
      if (page_data[i] != 0xFF) {
        has_data = true;
        break;
      }
    }

    if (has_data) {
      Serial.printf("Programming page %d (0x%04X)\n", page, addr);

      // Load page buffer
      for (uint32_t i = 0; i < page_size && (addr + i) < size; i += 2) {
        uint8_t low_byte = page_data[i];
        uint8_t high_byte = (addr + i + 1 < size) ? page_data[i + 1] : 0xFF;

        uint16_t word_addr = (addr + i) / 2;

        // Load program memory page (low byte)
        spiTransaction(0x40, 0x00, word_addr & 0xFF, low_byte);

        // Load program memory page (high byte)
        spiTransaction(0x48, 0x00, word_addr & 0xFF, high_byte);
      }

      // Write program memory page
      uint16_t page_addr = addr / 2;
      if (current_device.uses_word_addressing) {
        spiTransaction(0x4C, (page_addr >> 8) & 0xFF, page_addr & 0xFF, 0x00);
      } else {
        spiTransaction(0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0x00);
      }

      // Wait for page write to complete
      delay(10);
    }

    // Show progress every 10 pages
    if (page % 10 == 0) {
      showProgress(page, pages, "Programming");
    }
  }

  showProgress(pages, pages, "Programming");
  Serial.println("Flash programming complete");

  // Verify flash
  return verifyFlash(data, size);
}

bool ISPProgrammer::verifyFlash(const uint8_t* data, uint32_t size) {
  if (!data) return false;

  Serial.println("Verifying flash...");
  bool verify_ok = true;

  for (uint32_t addr = 0; addr < size; addr += 2) {
    uint16_t word_addr = addr / 2;

    // Read low byte
    uint8_t low_byte =
        spiTransaction(0x20, (word_addr >> 8) & 0xFF, word_addr & 0xFF, 0x00);

    // Read high byte
    uint8_t high_byte =
        spiTransaction(0x28, (word_addr >> 8) & 0xFF, word_addr & 0xFF, 0x00);

    // Compare with expected data
    if (data[addr] != low_byte) {
      Serial.printf(
          "Verify error at 0x%04X: expected 0x%02X, got 0x%02X (low)\n", addr,
          data[addr], low_byte);
      verify_ok = false;
      break;
    }

    if (addr + 1 < size && data[addr + 1] != high_byte) {
      Serial.printf(
          "Verify error at 0x%04X: expected 0x%02X, got 0x%02X (high)\n",
          addr + 1, data[addr + 1], high_byte);
      verify_ok = false;
      break;
    }

    // Show progress every 1KB
    if (addr % 1024 == 0) {
      showProgress(addr, size, "Verifying");
    }
  }

  if (verify_ok) {
    Serial.println("Flash verification successful!");
  } else {
    Serial.println("Flash verification failed!");
  }

  return verify_ok;
}

void ISPProgrammer::showProgress(uint32_t current, uint32_t total,
                                 const char* operation) {
  if (total == 0) return;
  uint32_t percent = (current * 100) / total;
  Serial.printf("%s progress: %d%%\n", operation, percent);
}

void ISPProgrammer::printDeviceInfo() const {
  Serial.printf("Device signature: 0x%02X 0x%02X 0x%02X\n",
                current_device.signature[0], current_device.signature[1],
                current_device.signature[2]);
  Serial.printf("Device: %s\n", current_device.name.c_str());

  if (device_detected) {
    Serial.printf("Page size: %d bytes\n", current_device.page_size);
    Serial.printf("Flash size: %d bytes\n", current_device.flash_size);
    Serial.printf("Addressing: %s\n",
                  current_device.uses_word_addressing ? "Word" : "Byte");
  }
}

void ISPProgrammer::printFuses() {
  if (!device_detected) {
    Serial.println("Device not detected or not in programming mode");
    return;
  }

  // Read fuses
  uint8_t lfuse = spiTransaction(0x50, 0x00, 0x00, 0x00);
  uint8_t hfuse = spiTransaction(0x58, 0x08, 0x00, 0x00);
  uint8_t efuse = spiTransaction(0x50, 0x08, 0x00, 0x00);

  Serial.printf("Fuses - Low: 0x%02X, High: 0x%02X, Extended: 0x%02X\n", lfuse,
                hfuse, efuse);
}

// Static device info methods
DeviceInfo ISPProgrammer::getATmega32U4Info() {
  DeviceInfo info;
  info.signature[0] = ATMEGA32U4_SIGNATURE_0;
  info.signature[1] = ATMEGA32U4_SIGNATURE_1;
  info.signature[2] = ATMEGA32U4_SIGNATURE_2;
  info.name = "ATmega32U4";
  info.page_size = ATMEGA32U4_PAGE_SIZE;
  info.flash_size = ATMEGA32U4_FLASH_SIZE;
  info.uses_word_addressing = true;
  return info;
}

DeviceInfo ISPProgrammer::getATmega328PInfo() {
  DeviceInfo info;
  info.signature[0] = ATMEGA328P_SIGNATURE_0;
  info.signature[1] = ATMEGA328P_SIGNATURE_1;
  info.signature[2] = ATMEGA328P_SIGNATURE_2;
  info.name = "ATmega328P";
  info.page_size = 128;
  info.flash_size = 32768;
  info.uses_word_addressing = false;
  return info;
}

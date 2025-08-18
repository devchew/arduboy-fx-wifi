# ESP8266 AVR ISP Programmer for ATmega32U4

This project implements an In-System Programmer (ISP) for programming ATmega32U4 microcontrollers using an ESP8266. It reads Intel HEX files from the ESP8266's LittleFS filesystem and programs them into the target AVR microcontroller.

## Features

- **Intel HEX Parsing**: Uses the arkku/ihex library for robust HEX file parsing
- **ISP Programming**: Programs ATmega32U4 via SPI interface using ESP_AVRISP library
- **File System Support**: Stores HEX files on ESP8266 LittleFS
- **Serial Commands**: Interactive command interface via serial monitor
- **Device Detection**: Reads and verifies device signatures
- **Flash Verification**: Verifies programmed data for reliability
- **Progress Indication**: Shows programming and verification progress

## Hardware Connections

Connect your ESP8266 to the ATmega32U4 as follows:

| ESP8266 Pin | ATmega32U4 Pin | Function |
|-------------|----------------|----------|
| D1 (GPIO5)  | RESET          | Reset    |
| D5 (GPIO14) | SCK            | Clock    |
| D7 (GPIO13) | MOSI           | Data Out |
| D6 (GPIO12) | MISO           | Data In  |
| GND         | GND            | Ground   |
| 3V3         | VCC            | Power*   |

*Only connect VCC if you want the ESP8266 to power the target. Otherwise, ensure the target is powered separately.

## Software Requirements

1. **PlatformIO IDE** or **Arduino IDE with ESP8266 support**
2. **ESP_AVRISP Library**: Automatically installed via platformio.ini
3. **Intel HEX Parser**: Included in this project (based on arkku/ihex)

## Installation

### Using PlatformIO (Recommended)

1. Install PlatformIO IDE or PlatformIO Core
2. Clone this repository
3. Open the project in PlatformIO
4. Build and upload to your ESP8266:

   ```bash
   platformio run --target upload
   ```

### Using Arduino IDE

1. Install ESP8266 board support in Arduino IDE
2. Install the ESP_AVRISP library from: <https://github.com/tostmann/ESP_AVRISP.git>
3. Copy the source files to your Arduino sketch folder
4. Compile and upload

## Usage

### 1. Upload HEX Files

First, you need to upload your Intel HEX files to the ESP8266's LittleFS filesystem. You can do this using:

- PlatformIO's filesystem uploader
- Arduino IDE's filesystem uploader plugin
- Or programmatically via WiFi (if WiFi is enabled)

### 2. Connect Hardware

Wire the ESP8266 to your ATmega32U4 target according to the connection table above.

### 3. Serial Commands

Open the serial monitor (115200 baud) and use these commands:

- `list` - Show files in LittleFS
- `info` - Display target device information
- `program <filename>` - Program a HEX file to the target
- `help` - Show available commands

### Example Session

```
ESP8266 AVR ISP Programmer
==========================
Target: ATmega32U4
LittleFS mounted successfully
Files in LittleFS:
  /firmware.hex (8432 bytes)
  /bootloader.hex (2048 bytes)

Commands:
  program <filename>  - Program HEX file to AVR
  list               - List files in LittleFS
  info               - Show AVR device info
  help               - Show this help

ISP Connections (ESP8266 -> ATmega32U4):
  D1 (GPIO5)  -> RESET
  D5 (GPIO14) -> SCK
  D7 (GPIO13) -> MOSI
  D6 (GPIO12) -> MISO
  GND         -> GND
  3V3         -> VCC (if powering target)

Ready> info
Device signature: 0x1E 0x95 0x87
Device: ATmega32U4
Fuses - Low: 0xFF, High: 0xD8, Extended: 0xCB

Ready> program firmware.hex
Programming file: /firmware.hex
Parsing HEX file: /firmware.hex
Data: 0x00000000, 16 bytes
Data: 0x00000010, 16 bytes
...
HEX file parsing complete
Flash size: 8432 bytes
Starting ISP programming...
Device signature: 0x1E 0x95 0x87
Erasing chip...
Programming flash...
Programming page 0 (0x0000)
Programming page 1 (0x0080)
...
Progress: 100%
Verifying flash...
Verify progress: 100%
Flash verification successful!
Programming complete!
Programming successful!
```

## Project Structure

```
├── platformio.ini          # PlatformIO configuration
├── src/
│   ├── main.cpp            # Main ISP programmer code
│   └── kk_ihex_read.c      # Intel HEX parser implementation
├── include/
│   └── kk_ihex_read.h      # Intel HEX parser header
└── README.md               # This file
```

## Key Components

### Intel HEX Parser

Based on the arkku/ihex library, provides robust parsing of Intel HEX format files with support for:

- Data records
- Extended address records
- End of file records
- Checksum validation

### ESP_AVRISP Integration

Uses the ESP_AVRISP library for low-level ISP communication:

- Device signature reading
- Flash programming with page-based writes
- Fuse bit reading
- Flash verification

### ATmega32U4 Support

Specifically configured for ATmega32U4 with:

- Device signature: 0x1E 0x95 0x87
- Page size: 128 bytes
- Flash size: 32KB
- Word-based addressing for flash operations

## Troubleshooting

### Programming Fails

1. Check all wiring connections
2. Ensure target is powered (3.3V or 5V)
3. Verify the target isn't in a locked state
4. Check that RESET line is properly connected

### File Not Found

1. Use `list` command to see available files
2. Ensure HEX file is uploaded to LittleFS
3. Use correct filename with leading slash (e.g., `/firmware.hex`)

### Device Signature Mismatch

1. Verify you're connecting to the correct target
2. Check that the target is an ATmega32U4
3. Ensure all ISP connections are secure

## License

This project uses libraries with different licenses:

- ESP_AVRISP: Check the library's repository for license
- arkku/ihex: MIT License
- This implementation: Use as needed for your projects

## Contributing

Feel free to submit issues and enhancement requests! This is a educational/hobbyist project for programming AVR microcontrollers.
  - Optional UI/status screen when the AVR is held in reset.
- **Single shared OLED screen** between AVR (for gameplay) and ESP32 (for menus).

The aim: **Drop a HEX file into the Web UI, flash, and play** on real hardware — no PC tools needed.

---

## 🛠️ Hardware (prototype)
- **SparkFun Pro Micro** (ATmega32u4, 5 V).
- **ESP32 module** (WROOM / DevKit).
- **2.42″ 128×64 OLED (SSD1309, SPI)**.
- LiPo + charger + 3.3 V regulator (for ESP32 + OLED).
- Simple level shifting (resistor dividers) for AVR→OLED.
- NPN transistor for ESP32 → AVR reset control.

---

## 🔄 Architecture Overview
- **AVR**:
  - Runs the game (compiled HEX).
  - Talks SPI to the OLED (through dividers).
- **ESP32**:
  - Holds AVR in reset when flashing or showing Wi-Fi UI.
  - Programs AVR via ISP (RESET, MOSI, MISO, SCK).
  - Hosts Web server for HEX upload.
  - Applies SSD1309 patch before flashing.
  - Can drive OLED directly when AVR is in reset.

---

## 🗺️ Roadmap (shallow)
## Phase 0 - Remote Programming
- [ ] let the esp32 flash the avr

### Phase 1 — Basic bring-up
- [ ] Wire up ESP32 ↔ Pro Micro ↔ OLED (per schematic).
- [ ] ESP32 drives OLED test pattern with AVR held in reset.
- [ ] ESP32 reads AVR signature via ISP (prove programming works).

### Phase 2 — End-to-end flashing
- [ ] Implement HEX upload over Web UI.
- [ ] Add SSD1309 patch routine on ESP32.
- [ ] Flash a game into AVR, release reset, verify gameplay on OLED.

### Phase 3 — Polish
- [ ] Add on-device game library (optional microSD or SPIFFS).
- [ ] Status UI on ESP32 (battery, Wi-Fi, game select).
- [ ] Clean up bus handover (tri-state buffers for stability).
- [ ] Case / PCB design.

### Phase 4 — Stretch goals
- [ ] Shared SPI flash like Arduboy FX for instant game switching.
- [ ] TFT support via ESP32 “SSD1306 emulation layer”.
- [ ] Online game library sync.

---

## 📜 License
TBD (MIT recommended).

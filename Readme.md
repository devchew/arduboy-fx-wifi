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

# Arduboy-FX WiFi — ESP-based AVR ISP & Web Uploader

Lightweight ESP8266/ESP32-based ISP and web uploader for ATmega32U4 (Arduboy-style) projects.

This repository contains firmware that runs on an ESP8266 (or ESP32 variants) to program an ATmega32U4 using ISP. Hex files are stored on the ESP filesystem (LittleFS) and can be flashed via serial commands or a simple web UI (prototype).

Key goals:

- Provide a small, reliable ISP programmer using cheap ESP modules
- Host/upload HEX files on-device (LittleFS)
- Offer serial and web-based flashing workflows for convenience

Supported hardware (examples):

- ESP8266 modules (NodeMCU, Wemos D1 mini)
- ESP32 modules (WROOM, DevKit) — may require small pin adjustments
- ATmega32U4-based boards (Pro Micro / SparkFun Pro Micro / etc.)
- Optional SPI OLED for UI (prototype)

Highlights

- Intel HEX parsing and verification
- ISP programming (read signature, erase, program, verify)
- LittleFS file storage for HEX files
- Serial CLI for local use and a prototype web uploader

Quick Start (PlatformIO)

1. Install PlatformIO in VS Code or PlatformIO Core.
2. Clone this repository and open it in PlatformIO.
3. Build and upload to your ESP board:

```bash
platformio run --target upload
```

4. Open the serial monitor at `115200` baud and use `help` for commands.

Typical Serial Commands

- `list` — list files on LittleFS
- `info` — read AVR signature, fuses, and device info
- `program <filename>` — program the given HEX file from LittleFS
- `help` — show CLI help

Hardware Wiring (ESP8266 example)

Connect pins between the ESP and the ATmega32U4 as follows (adjust GPIOs for your board):

- ESP -> ATmega32U4
- D1 (GPIO5)  -> RESET
- D5 (GPIO14) -> SCK
- D7 (GPIO13) -> MOSI
- D6 (GPIO12) -> MISO
- GND         -> GND
- 3V3         -> VCC (only if you want ESP to power the target)

Notes

- If the target is 5V, do not power it from the ESP's 3.3V regulator. Use a proper 5V supply and common ground.
- Use a level shifter or simple resistor dividers for MOSI/SCK when targeting 5V boards.

Filesystem (LittleFS)

Upload HEX files to LittleFS using PlatformIO's filesystem upload, the Arduino plugin, or tools included in this repo (prototype web uploader).

Project Layout

- `platformio.ini` — PlatformIO configuration and library dependencies
- `src/` — firmware source (ISP logic, filesystem, CLI)
- `include/` — headers
- `data/` — example HEX files ready for LittleFS upload

Troubleshooting

- If programming fails: check wiring, ensure RESET control works, and confirm the target is powered.
- If file not found: run `list` to check filenames and upload the HEX to LittleFS.
- If signature mismatch: confirm device is an ATmega32U4 and wiring is correct.

Contributing and Roadmap

This project is maintained as an educational/hobbyist tool. Contributions welcome — please open issues or PRs.

Short-term roadmap items:

- Improve web uploader UI and upload flow
- Add support for additional AVR devices and automatic pin mappings for ESP32/ESP8266
- Optional OLED status UI for standalone operation

License

Libraries used may have their own licenses (see `platformio.ini` and library docs). Project code is intended to be permissively licensed — recommend MIT (TBD).

Acknowledgements

- ESP_AVRISP and arkku/ihex implementations used as references


## Pin wiring table

| Arduboy function | Arduboy <BR>Leonardo/Micro |   DevelopmentKit    | ProMicro 5V <br>(standard wiring) | ProMicro 5V <br>(alternate wiring) |
| ---------------- | ---------------------- | ----------- | ---------------------------------- | --------------------------------- |
| OLED CS          | 12 PORTD6              |  6 PORTD7   |    GND/(inverted CART_CS)****      |  1/TXO PORTD3*                    |
| OLED DC          |  4 PORTD4              |  4 PORTD4   |  4 PORTD4                          |  4 PORTD4                         |
| OLED RST         |  6 PORTD7              | 12 PORTD6   |  6 PORTD7                          |  2 PORTD1*                        |
| SPI SCK          | 15 PORTB1              | 15 PORTB1   | 15 PORTB                   1       | 15 PORTB1                         |
| SPI MOSI         | 16 PORTB2              | 16 PORTB2   | 16 PORTB2                          | 16 PORTB2                         |
| RGB LED RED      | 10 PORTB6              |    _        | 10 PORTB6                          | 10 PORTB6                         |
| RGB LED GREEN    | 11 PORTB7              |    _        |    -                               |  3 PORTD0*                        |
| RGB LED BLUE     |  9 PORTB5              | 17 PORTB0   |  9 PORTB5                          |  9 PORTB5                         |
| RxLED            | 17 PORTB0              |    _        | 17 PORTB0                          | 17 PORTB0                         |
| TxLED            | 30 PORTD5              |    _        | 30 PORTD5                          | 30 PORTD5                         |
| BUTTON UP        | A0 PORTF7              |  8 PORTB4   | A0 PORTF7                          | A0 PORTF7                         |
| BUTTON RIGHT     | A1 PORTF6              |  5 PORTC6   | A1 PORTF6                          | A1 PORTF6                         |
| BUTTON LEFT      | A2 PORTF5              |  9 PORTB5   | A2 PORTF5                          | A2 PORTF5                         |
| BUTTON DOWN      | A3 PORTF4              | 10 PORTB6   | A3 PORTF4                          | A3 PORTF4                         |
| BUTTON A (left)  |  7 PORTE6              | A0 PORTF7   |  7 PORTE6                          |  7 PORTE6                         |
| BUTTON B (right) |  8 PORTB4              | A1 PORTF6   |  8 PORTB4                          |  8 PORTB4                         |
| SPEAKER PIN 1    |  5 PORTC6              | A2 PORTF5   |  5 PORTC6                          |  5 PORTC6                         |
| SPEAKER PIN 2    | 13 PORTC7              | A3 PORTF4** |    GND                             |  6 PORTD7*                        |

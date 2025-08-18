# Arduboy FX WiFi

## 🎯 Goal
This project is a **modernized Arduboy clone** with a bigger OLED screen and built-in Wi-Fi.  
Key features:
- **ATmega32u4** runs original Arduboy games unmodified.
- **ESP32** adds:
  - Web interface (upload `.hex` games directly from browser).
  - On-device HEX patching for **SSD1309** displays (common 2.42″ OLED).
  - AVR programmer (via ISP).
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

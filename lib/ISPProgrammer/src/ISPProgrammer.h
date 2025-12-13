#ifndef ISP_PROGRAMMER_H
#define ISP_PROGRAMMER_H

#include <Arduino.h>
#include <SPI.h>

// Device signatures
#define ATMEGA32U4_SIGNATURE_0  0x1E
#define ATMEGA32U4_SIGNATURE_1  0x95
#define ATMEGA32U4_SIGNATURE_2  0x87

#define ATMEGA328P_SIGNATURE_0  0x1E
#define ATMEGA328P_SIGNATURE_1  0x95
#define ATMEGA328P_SIGNATURE_2  0x0F

// ATmega32U4 specifications
#define ATMEGA32U4_PAGE_SIZE    128
#define ATMEGA32U4_FLASH_SIZE   32768

struct DeviceInfo {
    uint8_t signature[3];
    String name;
    uint32_t page_size;
    uint32_t flash_size;
    bool uses_word_addressing;
};

class ISPProgrammer {
private:
    uint8_t reset_pin;
    SPISettings spiSettings;
    DeviceInfo current_device;
    bool device_detected;
    
    bool detectDevice();
    bool verifyFlash(const uint8_t* data, uint32_t size);
    void showProgress(uint32_t current, uint32_t total, const char* operation);
    uint8_t spiTransaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

   public:
    ISPProgrammer(uint8_t reset_pin);
    ~ISPProgrammer();
    bool begin();
    void end();

    bool enterProgrammingMode();
    bool exitProgrammingMode();

    bool programFlash(const uint8_t* data, uint32_t size);
    bool eraseChip();

    DeviceInfo getDeviceInfo() const { return current_device; }
    bool isDeviceDetected() const { return device_detected; }

    void printDeviceInfo() const;
    void printFuses();

    // Static device info
    static DeviceInfo getATmega32U4Info();
    static DeviceInfo getATmega328PInfo();
};

#endif // ISP_PROGRAMMER_H

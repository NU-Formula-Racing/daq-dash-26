#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <nfr_can/IClock.hpp>
#include <nfr_can/IGpio.hpp>
#include <nfr_can/ISpi.hpp>

#include <glm/glm.hpp>

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <iostream>

namespace dash::platform {

class GPIO : public IGpio {
   public:
    GPIO(const std::string& chipName, unsigned lineOffset, bool output);
    ~GPIO();

    bool gpio_write(GpioLevel level) override;
    bool gpio_read(GpioLevel& out) override;

    bool checkError();

   private:
    struct GPIOImpl;
    std::unique_ptr<GPIOImpl> _impl;
};

class SPI : public ISpi {
   public:
    SPI(const std::string& device = "/dev/spidev0.0",
        uint32_t speedHz = 1'000'000,
        uint8_t mode = 0,
        uint8_t bitsPerWord = 8);
    ~SPI();

    bool ISpi_transfer(const uint8_t* tx, uint8_t* rx, size_t len) override;
    bool ISpi_write(const uint8_t* tx, size_t len) override;

   private:
    struct SPIImpl;
    std::unique_ptr<SPIImpl> _impl;
};

class Clock : public IClock {
   public:
    void sleepMs(uint32_t ms) override {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    uint32_t monotonicMs() override {
        using namespace std::chrono;
        return static_cast<uint32_t>(
            duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
    }
};

class NeopixelStrip {
   public:
    NeopixelStrip();
    ~NeopixelStrip();

    void init(const int& pin, const int& numLeds);
    void setColor(const int& ledIndex, const glm::vec4& color);
    void show();
    void cleanup();

   private:
    struct NeopixelImpl;
    std::unique_ptr<NeopixelImpl> _impl;
};

}  // namespace dash::platform

#endif  // __PLATFORM_H__

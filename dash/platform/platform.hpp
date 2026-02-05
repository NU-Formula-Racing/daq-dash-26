#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <nfr_can/IClock.hpp>
#include <nfr_can/IGpio.hpp>
#include <nfr_can/ISpi.hpp>

#include <chrono>
#include <memory>

namespace dash::platform {

class GPIO : public IGpio {
public:
  void gpio_write(GpioLevel level) override;
  GpioLevel gpio_read() override;

private:
  struct GPIOImpl;
  std::unique_ptr<GPIOImpl> _impl;
};

class SPI : public ISpi {
public:
  bool ISpi_transfer(const uint8_t *tx, uint8_t *rx, size_t len) override;
  bool ISpi_write(const uint8_t *tx, size_t len) override;

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
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
  }
};

}; // namespace dash::platform

#endif // __PLATFORM_H__
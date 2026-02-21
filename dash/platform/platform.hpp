#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <nfr_can/IClock.hpp>
#include <nfr_can/IGpio.hpp>
#include <nfr_can/ISpi.hpp>

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <iostream>
#include <functional>

namespace dash::platform {
struct GPIOError {
  bool chip_open_err = false;
  bool config_alloc_err = false;
  bool line_config_add_line_settings_err = false;
  bool gpiod_chip_request_lines_err = false;

  bool checkError() {
    return  chip_open_err || 
            config_alloc_err || 
            line_config_add_line_settings_err || 
            gpiod_chip_request_lines_err;
  }
};

class GPIO : public IGpio {
public:
  enum class EdgeType {
    RISING,
    FALLING,
    BOTH
  };

  GPIO(uint8_t pin, bool output);
  ~GPIO();

  bool gpio_write(GpioLevel level) override;
  bool gpio_read(GpioLevel& out) override;

  void attachInterrupt(std::function<void()> callback, EdgeType edge);

  bool checkError();

private:
  struct GPIOImpl;
  std::unique_ptr<GPIOImpl> _impl;
};

class SPI : public ISpi {
public:
  SPI(const std::string &device = "/dev/spidev0.0",
      uint32_t speedHz = 1'000'000, uint8_t mode = 0, uint8_t bitsPerWord = 8);
  ~SPI();

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
    using namespace std::chrono;
    return static_cast<uint32_t>(
        duration_cast<milliseconds>(steady_clock::now().time_since_epoch())
            .count());
  }
};

} // namespace dash::platform

#endif // __PLATFORM_H__

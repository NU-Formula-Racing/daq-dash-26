#include <platform/platform.hpp>

#include <cerrno>
#include <cstring>
#include <stdexcept>

// SPI (spidev)
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

// GPIO (libgpiod)
#include <gpiod.h>

namespace dash::platform {

// GPIO implementation

struct GPIO::GPIOImpl {
  gpiod_chip *_chip = nullptr;
  gpiod_line *_line = nullptr;
  bool _isOutput = false;

  GPIOImpl(const std::string &chipName, unsigned lineOffset, bool output)
      : _isOutput(output) {
    _chip = gpiod_chip_open_by_name(chipName.c_str());
    if (!_chip) {
      throw std::runtime_error("gpiod_chip_open_by_name failed: " +
                               std::string(std::strerror(errno)));
    }

    _line = gpiod_chip_get_line(_chip, lineOffset);
    if (!_line) {
      gpiod_chip_close(_chip);
      throw std::runtime_error("gpiod_chip_get_line failed: " +
                               std::string(std::strerror(errno)));
    }

    const char *consumer = "dash-platform";

    int rc = _isOutput ? gpiod_line_request_output(_line, consumer, 0)
                       : gpiod_line_request_input(_line, consumer);

    if (rc < 0) {
      gpiod_chip_close(_chip);
      throw std::runtime_error("gpiod_line_request failed: " +
                               std::string(std::strerror(errno)));
    }
  }

  ~GPIOImpl() {
    if (_line) {
      gpiod_line_release(_line);
    }
    if (_chip) {
      gpiod_chip_close(_chip);
    }
  }

  void write(GpioLevel level) {
    if (!_isOutput) {
      throw std::runtime_error("Attempted GPIO write on input line");
    }

    int value = (level == GpioLevel::G_HIGH) ? 1 : 0;
    if (gpiod_line_set_value(_line, value) < 0) {
      throw std::runtime_error("gpiod_line_set_value failed: " +
                               std::string(std::strerror(errno)));
    }
  }

  GpioLevel read() {
    int value = gpiod_line_get_value(_line);
    if (value < 0) {
      throw std::runtime_error("gpiod_line_get_value failed: " +
                               std::string(std::strerror(errno)));
    }

    return value ? GpioLevel::G_HIGH : GpioLevel::G_LOW;
  }
};

GPIO::GPIO(const std::string &chipName, unsigned lineOffset, bool output)
    : _impl(std::make_unique<GPIOImpl>(chipName, lineOffset, output)) {}

GPIO::~GPIO() = default;

void GPIO::gpio_write(GpioLevel level) { _impl->write(level); }

GpioLevel GPIO::gpio_read() { return _impl->read(); }

// SPI implementation

struct SPI::SPIImpl {
  int _fd = -1;
  uint32_t _speedHz = 0;
  uint8_t _mode = 0;
  uint8_t _bitsPerWord = 8;

  SPIImpl(const std::string &device, uint32_t speedHz, uint8_t mode,
          uint8_t bitsPerWord)
      : _speedHz(speedHz), _mode(mode), _bitsPerWord(bitsPerWord) {
    _fd = ::open(device.c_str(), O_RDWR | O_CLOEXEC);
    if (_fd < 0) {
      throw std::runtime_error("open(spidev) failed: " +
                               std::string(std::strerror(errno)));
    }

    if (ioctl(_fd, SPI_IOC_WR_MODE, &_mode) < 0 ||
        ioctl(_fd, SPI_IOC_WR_BITS_PER_WORD, &_bitsPerWord) < 0 ||
        ioctl(_fd, SPI_IOC_WR_MAX_SPEED_HZ, &_speedHz) < 0) {
      ::close(_fd);
      throw std::runtime_error("SPI configuration failed: " +
                               std::string(std::strerror(errno)));
    }
  }

  ~SPIImpl() {
    if (_fd >= 0) {
      ::close(_fd);
    }
  }

  bool transfer(const uint8_t *tx, uint8_t *rx, size_t len) {
    if (len == 0) {
      return true;
    }

    spi_ioc_transfer tr{};
    tr.tx_buf = reinterpret_cast<unsigned long>(tx);
    tr.rx_buf = reinterpret_cast<unsigned long>(rx);
    tr.len = static_cast<uint32_t>(len);
    tr.speed_hz = _speedHz;
    tr.bits_per_word = _bitsPerWord;

    return ioctl(_fd, SPI_IOC_MESSAGE(1), &tr) >= 0;
  }

  bool write(const uint8_t *tx, size_t len) {
    return transfer(tx, nullptr, len);
  }
};

SPI::SPI(const std::string &device, uint32_t speedHz, uint8_t mode,
         uint8_t bitsPerWord)
    : _impl(std::make_unique<SPIImpl>(device, speedHz, mode, bitsPerWord)) {}

SPI::~SPI() = default;

bool SPI::ISpi_transfer(const uint8_t *tx, uint8_t *rx, size_t len) {
  return _impl->transfer(tx, rx, len);
}

bool SPI::ISpi_write(const uint8_t *tx, size_t len) {
  return _impl->write(tx, len);
}

} // namespace dash::platform

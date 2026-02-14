// mock platform

#include <platform/platform.hpp>

#include <cstring>

namespace dash::platform {

struct GPIO::GPIOImpl {
  GpioLevel _level = GpioLevel::G_UNDEF;
};

GPIO::GPIO(const std::string &, unsigned, bool)
    : _impl(std::make_unique<GPIOImpl>()) {}

GPIO::~GPIO() = default;

bool GPIO::gpio_write(GpioLevel level) { 
  _impl->_level = level; 
  return true;
}
bool GPIO::gpio_read(GpioLevel& out){ 
  out = _impl->_level; 
  return true;
}
bool GPIO::checkError() { return false; }

struct SPI::SPIImpl {
  // noop
};

SPI::SPI(const std::string &, uint32_t, uint8_t, uint8_t)
    : _impl(std::make_unique<SPIImpl>()) {}

SPI::~SPI() = default;

bool SPI::ISpi_write(const uint8_t *, size_t) { return false; }

bool SPI::ISpi_transfer(const uint8_t *tx, uint8_t *rx, size_t len) {
    return false;
}

} // namespace dash::platform

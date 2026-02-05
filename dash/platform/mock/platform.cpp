// mock platform

#include <dash/platform/platform.hpp>

namespace dash::platform {

struct GPIO::GPIOImpl {
    // noop
}; 

void GPIO::gpio_write(GpioLevel level) {
    // noop
}

GpioLevel GPIO::gpio_read() {
    return GpioLevel::G_UNDEF;
}

struct SPI::SPIImpl {
    // noop
};

bool SPI::ISpi_write(const uint8_t* tx, size_t len) {
    return true;
}

bool SPI::ISpi_transfer(const uint8_t* tx, uint8_t* rx, size_t len) {
    return true;
}

};
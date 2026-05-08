#include <platform/interfaces.hpp>

namespace dash {

struct GPIO::GPIOImpl {
    GpioLevel _level = GpioLevel::G_UNDEF;
};

GPIO::GPIO(uint8_t, bool) : _impl(std::make_unique<GPIOImpl>()) {}

GPIO::~GPIO() = default;

bool GPIO::gpio_write(GpioLevel level) {
    _impl->_level = level;

    return true;
}

bool GPIO::gpio_read(GpioLevel& out) {
    out = _impl->_level;

    return true;
}

void GPIO::attachInterrupt(std::function<void()> callback, EdgeType edge) {}

bool GPIO::checkError() {
    return true;
}

};  // namespace dash

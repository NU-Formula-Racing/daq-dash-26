#include <atomic>
#include <cerrno>
#include <cstring>
#include <gpiod.hpp>
#include <iostream>
#include <map>
#include <platform/interfaces.hpp>
#include <platform/rpi/gpio_manager.hpp>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace dash {

struct GPIO::GPIOImpl {
    uint8_t _pin;
    bool _isOutput;
    gpiod::line_settings _settings;
    bool err;

    GPIOImpl(uint8_t pin, bool output) : _pin(pin), _isOutput(output) {
        _settings.set_direction(output ? gpiod::line::direction::OUTPUT
                                       : gpiod::line::direction::INPUT);

        if (!output) {
            _settings.set_edge_detection(gpiod::line::edge::BOTH);
        }

        err = GPIOManager::instance().registerPin(_pin, _settings);
    }

    ~GPIOImpl() { GPIOManager::instance().releasePin(_pin); }

    bool write(GpioLevel level) {
        if (!_isOutput)
            return false;

        return GPIOManager::instance().gpioWritePin(_pin, level);
    }

    bool read(GpioLevel& out) {
        if (_isOutput)
            return false;

        return GPIOManager::instance().gpioReadPin(_pin, out);
    }

    void attachInterrupt(std::function<void()> callback, EdgeType edge) {
        if (_isOutput)
            return;

        GPIOManager::instance().registerInterrupt(_pin, _settings, callback, edge);
    }

    bool checkError() { return err; }
};

GPIO::GPIO(uint8_t pin, bool output) : _impl(std::make_unique<GPIOImpl>(pin, output)) {
}

GPIO::~GPIO() = default;

bool GPIO::gpio_write(GpioLevel level) {
    return _impl->write(level);
}

bool GPIO::gpio_read(GpioLevel& out) {
    return _impl->read(out);
}

void GPIO::attachInterrupt(std::function<void()> callback, EdgeType edge) {
    _impl->attachInterrupt(std::move(callback), edge);
}

bool GPIO::checkError() {
    return _impl->checkError();
}

}  // namespace dash

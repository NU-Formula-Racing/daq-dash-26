#include <platform/platform.hpp>

#include <gpiod.hpp>

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>
#include <iostream>
#include <thread>
#include <atomic>
#include <map>
#include <vector>


namespace dash::platform {

struct GPIO::GPIOImpl {
  uint8_t _pin;
  bool _isOutput;
  gpiod::line_settings _settings;

  GPIOImpl(uint8_t pin, bool output) 
      : _pin(pin), _isOutput(output) {
    
    _settings.set_direction(output ? gpiod::line::direction::OUTPUT 
                                      : gpiod::line::direction::INPUT);
    
    GPIOManager::instance()->registerPin(_pin, _settings);
  }

  ~GPIOImpl() {
    GPIOManager::instance()->unregisterPin(_pin);
  }

  bool write(GpioLevel level) {
    static_assert(_isOutput, "cannot write to GPIO input pin");

    return GPIOManager::instance()->gpioWritePin(_pin, level);
  }

  bool read(GpioLevel& out) {
    static_assert(!_isOutput, "cannot read from GPIO output pin");

    return GPIOManager::instance()->gpioReadPin(_pin, out);
  }

  void attachInterrupt(std::function<void()> callback, EdgeType edge){
    static_assert(!_isOutput, "cannot attach interrupt to GPIO output pin");

    if (edge == EdgeType::RISING) {
      _settings.set_edge_detection(gpiod::line::edge::RISING);
    } else if (edge == EdgeType::FALLING) {
      _settings.set_edge_detection(gpiod::line::edge::FALLING);
    } else {
      _settings.set_edge_detection(gpiod::line::edge::BOTH);
    }
    
    GPIOManager::instance()->registerInterrupt(_pin, _settings, callback);
  }
  
};

GPIO::GPIO(uint8_t pin, bool output)
    : _impl(std::make_unique<GPIOImpl>(uint8_t pin, bool output)) {}

GPIO::~GPIO() = default;

bool GPIO::gpio_write(GpioLevel level) {
  return _impl->write(level);
}

bool GPIO::gpio_read(GpioLevel& out) {
  return _impl->read(out);
}

void GPIO::attachInterrupt(std::function<void()> callback, EdgeType edge){
  _impl->attachInterrupt(std::function<void()> callback, EdgeType edge);
}

} // namespace dash::platform

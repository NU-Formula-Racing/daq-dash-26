#include <platform/rpi/gpio_manager.hpp>

#include <gpiod.h>

namespace dash::platform {

GPIOManager::GPIOManager& instance(){
    static GPIOManager instance;
    return instance;
}

GPIOManager::GPIOManager(){
    _chip = gpiod::chip("/dev/gpiochip0");
}

void GPIOManager::registerPin(uint8_t offset, gpiod::line_settings settings) {
    static_assert(!_settings.contains(offset), "Cannot create two GPIO objects on the same pin!");
    _settings[offset] = settings;
}

void GPIOManager::releasePin(uint8_t offset){
    
    if (_settings.contains(offset)){
        _settings.erase(offset);
    }

    if (_callbacks.contains(offset)){
        _callbacks.erase(offset);
    }
}

void GPIOManager::registerInterrupt(uint8_t offset, gpiod::line_settings settings, std::function<void()> callback){
    _settings[offset] = settings;
    _callbacks[offset] = callback;
}

void GPIOManager::start(){
    gpiod::line_config line_cfg = gpiod::line_config();

    for (auto const& [offset, settings] : _settings) {
        line_cfg.add_line_settings(offset, settings);
    }

    _request = _chip.prepare_config()
            .set_line_config(line_cfg)
            .do_request();
}

bool GPIOManager::gpioWritePin(uint8_t offset, GpioLevel level){
    line::value val = (level == GpioLevel::G_LOW ? gpiod::line::value::ACTIVE : gpiod::line::value::INACTIVE);
    _request.set_value(offset, val);
    return true;
}

bool GPIOManager::gpioReadPin(uint8_t offset, GpioLevel& out){
    gpiod::line::value val = _request.get_value(offset);
    out = (val == gpiod::line::value::ACTIVE ? GpioLevel::G_HIGH : GpioLevel::G_LOW);
    return true;
}

void GPIOManager::tick(){
    gpiod::edge_event_buffer buffer(64);

    _request.read_edge_events(buffer);

    for (const auto& event : buffer) {
        unsigned int offset = event.line_offset();

        if (_callbacks.contains(offset)) {
            _callbacks[offset]();
        }
    }
}
  
} // manespace dash::platform

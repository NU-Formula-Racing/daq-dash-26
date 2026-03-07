#include <platform/rpi/gpio_manager.hpp>

#include <gpiod.hpp>

namespace dash::platform {

GPIOManager& GPIOManager::instance(){
    static GPIOManager instance;
    return instance;
}

GPIOManager::GPIOManager() :
    _chip(std::make_unique<gpiod::chip>("/dev/gpiochip0")) {}

bool GPIOManager::registerPin(uint8_t offset, gpiod::line_settings settings) {
    if (_settings.find(offset) != _settings.end()) {
        return false;
    }
    _settings[offset] = settings;
    return true;
}

void GPIOManager::releasePin(uint8_t offset){
    
    if (_settings.find(offset) != _settings.end()){
        _settings.erase(offset);
    }

    if (_risingCallbacks.find(offset) != _risingCallbacks.end()){
        _risingCallbacks.erase(offset);
    }

    if (_fallingCallbacks.find(offset) != _fallingCallbacks.end()){
        _fallingCallbacks.erase(offset);
    }
}

void GPIOManager::registerInterrupt(uint8_t offset, gpiod::line_settings settings, std::function<void()> callback, EdgeType edge){
    _settings[offset] = settings;

    if (edge == EdgeType::FALLING || edge == EdgeType::BOTH){
        _fallingCallbacks[offset] = callback;
    } 

    if (edge == EdgeType::RISING || edge == EdgeType::BOTH){
        _risingCallbacks[offset] = callback;
    }
    
}

void GPIOManager::start(){
    gpiod::line_config line_cfg = gpiod::line_config();

    for (auto const& [offset, settings] : _settings) {
        line_cfg.add_line_settings(offset, settings);
    }

    _request = std::make_unique<gpiod::line_request>(
        _chip->prepare_request().set_line_config(line_cfg).do_request()
    );
}

bool GPIOManager::gpioWritePin(uint8_t offset, GpioLevel level){
    gpiod::line::value val = (level == GpioLevel::G_LOW ? gpiod::line::value::INACTIVE : gpiod::line::value::ACTIVE);
    _request->set_value(offset, val);
    return true;
}

bool GPIOManager::gpioReadPin(uint8_t offset, GpioLevel& out){
    gpiod::line::value val = _request->get_value(offset);
    out = (val == gpiod::line::value::ACTIVE ? GpioLevel::G_HIGH : GpioLevel::G_LOW);
    return true;
}

void GPIOManager::tick(){
    if (!_started) {
        _started = true;
        start();
    }

    if (!_request) {
        return;
    }

    if (!_request->wait_edge_events(std::chrono::nanoseconds(0))) {
        return;
    }

    gpiod::edge_event_buffer buffer(64);
    std::size_t num_events = _request->read_edge_events(buffer);

    for (std::size_t i = 0; i < num_events; i++) {
        const auto& event = buffer.get_event(i);
        uint8_t offset = static_cast<uint8_t>(event.line_offset());

        if (event.type() == gpiod::edge_event::event_type::RISING_EDGE) {
            auto it = _risingCallbacks.find(offset);
            if (it != _risingCallbacks.end()) {
                it->second();
            }
        } else if (event.type() == gpiod::edge_event::event_type::FALLING_EDGE) {
            auto it = _fallingCallbacks.find(offset);
            if (it != _fallingCallbacks.end()) {
                it->second();
            }
        }
    }
}
  
} // manespace dash::platform

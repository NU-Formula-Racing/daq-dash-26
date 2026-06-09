#include <okay/okay.hpp>

#include <chrono>
#include <cstdint>
#include <functional>
#include <gpiod.hpp>
#include <memory>
#include <platform/rpi/gpio_manager.hpp>
#include <thread>

namespace {

template <typename Fn>
bool retryGpioRequest(Fn&& fn) {
    constexpr int maxAttempts = 50;
    constexpr auto delay = std::chrono::milliseconds(100);

    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        try {
            fn();
            return true;
        } catch (const std::exception& e) {
            okay::Engine.logger.error(
                "GPIO line request failed: {}. Attempt {}/{}", e.what(), attempt + 1, maxAttempts);
        }

        std::this_thread::sleep_for(delay);
    }

    return false;
}

}  // namespace

namespace dash {

GPIOManager& GPIOManager::instance() {
    static GPIOManager instance;
    return instance;
}

GPIOManager::GPIOManager() : _chip(std::make_unique<gpiod::chip>("/dev/gpiochip0")) {}

bool GPIOManager::registerPin(uint8_t offset, gpiod::line_settings settings) {
    if (_settings.find(offset) != _settings.end()) {
        return false;
    }

    _settings[offset] = settings;

    if (_request) {
        rebuildRequest();
    }

    return true;
}

void GPIOManager::releasePin(uint8_t offset) {
    _settings.erase(offset);
    _risingCallbacks.erase(offset);
    _fallingCallbacks.erase(offset);

    if (_request) {
        rebuildRequest();
    }
}

void GPIOManager::registerInterrupt(uint8_t offset,
    gpiod::line_settings settings,
    std::function<void()> callback,
    GPIO::EdgeType edge) {
    _settings[offset] = settings;

    if (edge == GPIO::EdgeType::FALLING || edge == GPIO::EdgeType::BOTH) {
        _fallingCallbacks[offset] = callback;
    }

    if (edge == GPIO::EdgeType::RISING || edge == GPIO::EdgeType::BOTH) {
        _risingCallbacks[offset] = callback;
    }

    if (_request) {
        rebuildRequest();
    }
}

void GPIOManager::rebuildRequest() {
    _request.reset();  // releases old GPIO request first

    if (_settings.empty()) {
        _started = false;
        return;
    }

    bool success = retryGpioRequest([this]() {
        gpiod::line_config line_cfg;

        for (auto const& [offset, settings] : _settings) {
            line_cfg.add_line_settings(offset, settings);
        }

        _request = std::make_unique<gpiod::line_request>(
            _chip->prepare_request().set_consumer("dash").set_line_config(line_cfg).do_request());
    });

    if (!success) {
        okay::Engine.logger.error("GPIO request failed after retries");
        _request.reset();
        _started = false;
        return;
    }

    _started = true;
}

void GPIOManager::start() {
    if (_request) {
        return;
    }

    rebuildRequest();

    if (!_request) {
        okay::Engine.logger.error("GPIOManager failed to start");
    }
}

bool GPIOManager::gpioWritePin(uint8_t offset, GpioLevel level) {
    if (!_request) {
        start();
        _started = true;
    }

    if (!_request) {
        return false;
    }

    gpiod::line::value val =
        (level == GpioLevel::G_LOW ? gpiod::line::value::INACTIVE : gpiod::line::value::ACTIVE);

    try {
        _request->set_value(offset, val);
    } catch (const std::exception& e) {
        okay::Engine.logger.error("Failed to write GPIO {}: {}", offset, e.what());
        return false;
    }

    return true;
}

bool GPIOManager::gpioReadPin(uint8_t offset, GpioLevel& out) {
    if (!_request) {
        start();
        _started = true;
    }

    if (!_request) {
        return false;
    }

    try {
        gpiod::line::value val = _request->get_value(offset);
        out = (val == gpiod::line::value::ACTIVE ? GpioLevel::G_HIGH : GpioLevel::G_LOW);
    } catch (const std::exception& e) {
        okay::Engine.logger.error("Failed to read GPIO {}: {}", offset, e.what());
        return false;
    }

    return true;
}

void GPIOManager::tick() {
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
    std::size_t numEvents = _request->read_edge_events(buffer);

    for (std::size_t i = 0; i < numEvents; i++) {
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

}  // namespace dash

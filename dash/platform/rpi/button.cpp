#include <okay/okay.hpp>

#include <platform/button.hpp>
#include <platform/input_manager.hpp>
#include <platform/interfaces.hpp>

namespace dash {

inline const std::uint32_t BUTTON_DEBOUNCE_TIME = 100;

Button::Button(uint8_t gpioPin)
    : _buttonID(gpioPin), _gpio(std::make_unique<GPIO>(gpioPin, false)) {
    InputManager::instance().registerButton(_buttonID);

    _gpio->attachInterrupt(
        [this, id = _buttonID]() {
            std::uint32_t now = okay::Engine.time->timeSinceStartMs();
            if (now - _lastUp < BUTTON_DEBOUNCE_TIME)
                return;

            _lastUp = now;

            InputManager::instance().executeUpCallbacks(id);
        },
        GPIO::EdgeType::FALLING);

    _gpio->attachInterrupt(
        [this, id = _buttonID]() {
            std::uint32_t now = okay::Engine.time->timeSinceStartMs();
            if (now - _lastDown < BUTTON_DEBOUNCE_TIME)
                return;

            _lastDown = now;
            InputManager::instance().executeDownCallbacks(id);
        },
        GPIO::EdgeType::RISING);
}

Button::~Button() {
    InputManager::instance().unregisterButton(_buttonID);
}

void Button::onDown(std::function<void()> callback) {
    InputManager::instance().attachDownCallback(_buttonID, std::move(callback));
}

void Button::onUp(std::function<void()> callback) {
    InputManager::instance().attachUpCallback(_buttonID, std::move(callback));
}

bool Button::isDownThisFrame() {
    return InputManager::instance().isDownThisFrame(_buttonID);
}

bool Button::isUpThisFrame() {
    return InputManager::instance().isUpThisFrame(_buttonID);
}

bool Button::isDown() {
    return InputManager::instance().isDown(_buttonID);
}

}  // namespace dash

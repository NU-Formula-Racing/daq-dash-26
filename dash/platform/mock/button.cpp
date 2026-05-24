#include <platform/button.hpp>

namespace dash {

Button::Button(uint8_t gpioPin)
    : _buttonID(gpioPin), _gpio(std::make_unique<GPIO>(gpioPin, false)) {}

Button::~Button() = default;

void Button::onDown(std::function<void()> callback) {}

void Button::onUp(std::function<void()> callback) {}

bool Button::isDownThisFrame() {
    return false;
}
bool Button::isUpThisFrame() {
    return false;
}
bool Button::isDown() {
    return false;
}

};  // namespace dash
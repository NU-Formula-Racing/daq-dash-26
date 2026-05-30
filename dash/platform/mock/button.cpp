#include <platform/button.hpp>
#include "glfw_input.hpp"
#include "platform/input_manager.hpp"
#include "GLFW/glfw3.h"

namespace dash {

Button::Button(uint8_t gpioPin)
    : _buttonID(gpioPin), _gpio(std::make_unique<GPIO>(gpioPin, false)) {}

Button::~Button() = default;

void Button::onDown(std::function<void()> callback) {
    InputManager::instance().attachDownCallback(_buttonID, callback);
}

void Button::onUp(std::function<void()> callback) {
    InputManager::instance().attachUpCallback(_buttonID, callback);
}

bool Button::isDownThisFrame() {
    return false;
}
bool Button::isUpThisFrame() {
    return false;
}
bool Button::isDown() {
    switch (_buttonID) {
       case 20:
        return GlfwInput::isKeyDown(GLFW_KEY_DOWN);
       case 16:
        return GlfwInput::isKeyDown(GLFW_KEY_LEFT);
    }
    return false;
}

};  // namespace dash

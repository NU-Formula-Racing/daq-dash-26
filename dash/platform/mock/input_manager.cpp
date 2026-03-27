#include <platform/input_manager.hpp>

namespace dash {

InputManager::InputManager() = default;

InputManager& InputManager::instance() {
    static InputManager instance;

    return instance;
}

void InputManager::registerButton(uint8_t buttonID) {
}

void InputManager::unregisterButton(uint8_t buttonID) {
}

void InputManager::attachDownCallback(uint8_t buttonID, std::function<void()> callback) {
}

void InputManager::attachUpCallback(uint8_t buttonID, std::function<void()> callback) {
}

void InputManager::executeDownCallbacks(uint8_t buttonID) {
}

void InputManager::executeUpCallbacks(uint8_t buttonID) {
}

void InputManager::registerEncoder(uint16_t encoderID, uint8_t leftPin, uint8_t rightPin) {
}

void InputManager::unregisterEncoder(uint16_t encoderID) {
}

void InputManager::attachLeftCallback(uint16_t encoderID, std::function<void()> callback) {
}

void InputManager::attachRightCallback(uint16_t encoderID, std::function<void()> callback) {
}

void InputManager::executeLeftCallbacks(uint16_t encoderID) {
}

void InputManager::executeRightCallbacks(uint16_t encoderID) {
}

void InputManager::onEncoderEdge(uint16_t encoderID) {
}

bool InputManager::isDownThisFrame(uint8_t buttonID) {
    return false;
}

bool InputManager::isUpThisFrame(uint8_t buttonID) {
    return false;
}

bool InputManager::isDown(uint8_t buttonID) {
    return false;
}

bool InputManager::isRightThisFrame(uint16_t encoderID) const {
    return false;
}

bool InputManager::isLeftThisFrame(uint16_t encoderID) const {
    return false;
}

bool InputManager::isIdle(uint16_t encoderID) const {
    return true;
}

void InputManager::tick() {
}

};  // namespace dash
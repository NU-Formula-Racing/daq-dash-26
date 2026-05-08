#include <platform/encoder.hpp>

namespace dash {

constexpr uint16_t Encoder::generateID(uint8_t left, uint8_t right) {
    return static_cast<uint16_t>((static_cast<uint16_t>(left) << 8U) | right);
}

Encoder::Encoder(uint8_t leftPin, uint8_t rightPin)
    : _encoderID(generateID(leftPin, rightPin)),
      _leftGPIO(std::make_unique<GPIO>(leftPin, false)),
      _rightGPIO(std::make_unique<GPIO>(rightPin, false)) {}

Encoder::~Encoder() = default;

void Encoder::onRight(std::function<void()> callback) {}

void Encoder::onLeft(std::function<void()> callback) {}

bool Encoder::isIdle() {
    return true;
}

bool Encoder::isRightThisFrame() {
    return false;
}

bool Encoder::isLeftThisFrame() {
    return false;
}

};  // namespace dash
#include <okay/okay.hpp>

#include <GLFW/glfw3.h>
#include <functional>
#include <memory>
#include <platform/encoder.hpp>

namespace dash {

struct EncoderState {
    std::function<void()> onRight;
    std::function<void()> onLeft;
};

static EncoderState s_encoderState;

static GLFWwindow* getWindow() {
    return (GLFWwindow*)okay::Engine.systems.getSystemChecked<okay::Renderer>()->getSurfaceWindow();
}

static void encoderScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0.0) {
        if (s_encoderState.onRight) {
            s_encoderState.onRight();
        }
    } else if (yoffset < 0.0) {
        if (s_encoderState.onLeft) {
            s_encoderState.onLeft();
        }
    }
}

static void ensureEncoderScrollCallbackInstalled() {
    static bool installed = false;

    if (!installed) {
        glfwSetScrollCallback(getWindow(), encoderScrollCallback);
        installed = true;
    }
}

constexpr uint16_t Encoder::generateID(uint8_t left, uint8_t right) {
    return static_cast<uint16_t>((static_cast<uint16_t>(left) << 8U) | right);
}

Encoder::Encoder(uint8_t leftPin, uint8_t rightPin)
    : _encoderID(generateID(leftPin, rightPin)),
      _leftGPIO(std::make_unique<GPIO>(leftPin, false)),
      _rightGPIO(std::make_unique<GPIO>(rightPin, false)) {}

Encoder::~Encoder() = default;

void Encoder::onRight(std::function<void()> callback) {
    ensureEncoderScrollCallbackInstalled();
    s_encoderState.onRight = std::move(callback);
}

void Encoder::onLeft(std::function<void()> callback) {
    ensureEncoderScrollCallbackInstalled();
    s_encoderState.onLeft = std::move(callback);
}

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

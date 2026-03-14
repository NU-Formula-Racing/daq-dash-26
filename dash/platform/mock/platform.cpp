// mock platform
#include "platform/platform.hpp"
#include <can/mock/can_imgui.hpp>

#include <cstring>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace dash::platform {

struct GPIO::GPIOImpl {
  GpioLevel _level = GpioLevel::G_UNDEF;
};

GPIO::GPIO(uint8_t, bool)
    : _impl(std::make_unique<GPIOImpl>()) {}

GPIO::~GPIO() = default;

bool GPIO::gpio_write(GpioLevel level) { 
  _impl->_level = level; 
  return true;
}
bool GPIO::gpio_read(GpioLevel& out){ 
  out = _impl->_level; 
  return true;
}

void GPIO::attachInterrupt(std::function<void()> callback, EdgeType edge) {}

bool GPIO::checkError(){ return true; }

Button::Button(uint8_t gpioPin) : _buttonID(gpioPin), _gpio(std::make_unique<GPIO>(gpioPin, false)) {}
Button::~Button() = default;

void Button::onDown(std::function<void()> callback) {}
void Button::onUp(std::function<void()> callback) {}

bool Button::isDownThisFrame() { return false; }
bool Button::isUpThisFrame() { return false; }
bool Button::isDown() { return false; }

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

bool Encoder::isIdle() { return true; }
bool Encoder::isRightThisFrame() { return false; }
bool Encoder::isLeftThisFrame() { return false; }

InputManager::InputManager() = default;

InputManager& InputManager::instance() {
  static InputManager instance;
  return instance;
}

void InputManager::registerButton(uint8_t buttonID) {}
void InputManager::unregisterButton(uint8_t buttonID) {}

void InputManager::attachDownCallback(uint8_t buttonID, std::function<void()> callback) {}
void InputManager::attachUpCallback(uint8_t buttonID, std::function<void()> callback) {}

void InputManager::executeDownCallbacks(uint8_t buttonID) {}
void InputManager::executeUpCallbacks(uint8_t buttonID) {}

void InputManager::registerEncoder(uint16_t encoderID, uint8_t leftPin, uint8_t rightPin) {}
void InputManager::unregisterEncoder(uint16_t encoderID) {}

void InputManager::attachLeftCallback(uint16_t encoderID, std::function<void()> callback) {}
void InputManager::attachRightCallback(uint16_t encoderID, std::function<void()> callback) {}

void InputManager::executeLeftCallbacks(uint16_t encoderID) {}
void InputManager::executeRightCallbacks(uint16_t encoderID) {}

void InputManager::onEncoderEdge(uint16_t encoderID) {}

bool InputManager::isDownThisFrame(uint8_t buttonID) { return false; }
bool InputManager::isUpThisFrame(uint8_t buttonID) { return false; }
bool InputManager::isDown(uint8_t buttonID) { return false; }

bool InputManager::isRightThisFrame(uint16_t encoderID) { return false; }
bool InputManager::isLeftThisFrame(uint16_t encoderID) { return false; }
bool InputManager::isIdle(uint16_t encoderID) { return true; }

void InputManager::tick() {}

struct SPI::SPIImpl {
  // noop
};

SPI::SPI(const std::string &, uint32_t, uint8_t, uint8_t)
    : _impl(std::make_unique<SPIImpl>()) {}

SPI::~SPI() = default;

bool SPI::ISpi_write(const uint8_t *, size_t) { return false; }

bool SPI::ISpi_transfer(const uint8_t *tx, uint8_t *rx, size_t len) {
    return false;
}

struct NeopixelStrip::NeopixelImpl {

};

NeopixelStrip::NeopixelStrip() : _impl(std::make_unique<NeopixelStrip::NeopixelImpl>()) {}
NeopixelStrip::~NeopixelStrip() {} 

void NeopixelStrip::init(const int& pin, const int &numLeds) {
  // noop
}

void NeopixelStrip::setColor(const int& ledIndex, const glm::vec4 &color) {
  // noop
}

void NeopixelStrip::show() {
  // noop
}

void NeopixelStrip::cleanup() {
  // noop
}

void configureCANDriver(CAN_Bus& bus) {
    auto canImgui = std::make_unique<CAN_IMGUI>();
    bus.set_driver(std::move(canImgui));
}

void preUpdate() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void postUpdate() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace dash::platform

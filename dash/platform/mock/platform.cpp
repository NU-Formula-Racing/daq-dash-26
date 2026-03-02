// mock platform

#include <memory>
#include <platform/platform.hpp>
#include <can/mock/can_imgui.hpp>
#include <can/can_dbc.hpp>

#include <cstring>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

bool checkError(){ return true; }

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

void configureCANDriver() {
    auto canImgui = std::make_unique<CAN_IMGUI>();
    dbc::driveBus.set_driver(std::move(canImgui));
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

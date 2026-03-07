// mock platform
#include "platform/platform.hpp"
#include <can/mock/can_imgui.hpp>

#include <cstring>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <okay/core/okay.hpp>
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
  int pin;
  int numLeds;
};

NeopixelStrip::NeopixelStrip() : _impl(std::make_unique<NeopixelStrip::NeopixelImpl>()) {}
NeopixelStrip::~NeopixelStrip() {} 

void NeopixelStrip::init(const int& pin, const int &numLeds) {
  // noop
  /*
    Initialize boxes for each strip
    Somehow choose orientation
    Initial color is black

  */
  _impl->pin = pin;
  _impl->numLeds = numLeds;
}

void NeopixelStrip::setColor(const int& ledIndex, const glm::vec4 &color) {
  // noop
  /*
    Update specific color of UI
  */
}

void NeopixelStrip::show() {
  // noop
  // okay::Engine.logger.debug("testing");
  // ImGui::Begin("Neopixel");
  // ImGui::Text("Testing");
  // ImGui::End();
  /*
    Draw UI
  */
  // ImGui::ShowDemoWindow();
  // ImGui::NewFrame();
  // ImGui::BeginChild("testing");
  // ImGui::Text("Neopixel");
  // ImGui::EndChild();
  // ImGuiIO& io = ImGui::GetIO();
  // io.DisplaySize = ImVec2((float)500, (float)500);
  // // ImGui::NewFrame();
  ImGui::Begin("Neopixel");
  
  
  auto ledBoxes = ImGui::GetWindowDrawList();
  uint8_t cols = _impl->numLeds / 8;
  
  if (ImGui::BeginTable("Neopixel Strip", 8)) {
    for(int i = 0; i < cols; i++) {
      for(int j = 0; j < _impl->numLeds / cols; j+=2) {
        ImGui::Text("testing");
        ImGui::TableNextRow();
      }
      ImGui::TableNextColumn();
    }
    
   }
  ImGui::EndTable();
  
  ImGui::End();
  // ImGui::EndFrame();
  // ImGui::Render();
  // ledBoxes->AddQuadFilled({})
  
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
  ImGui::EndFrame();
  ImGui::Render();
           
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace dash::platform

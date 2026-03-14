// mock platform
#include "platform/platform.hpp"
#include <can/mock/can_imgui.hpp>

#include <cstdarg>
#include <cstring>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <okay/core/okay.hpp>
#include <string>
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
  std::vector<ImColor> colors;
  
  void drawLedStrips(ImVec2 pos, int row, int col, bool rightToLeft) {
      if(row * col > numLeds) {
        okay::Engine.logger.error("Provided too many rows or columns.");
      }
      auto* drawList = ImGui::GetWindowDrawList();
      ImVec2 windowPos = ImGui::GetWindowPos();
      windowPos.x = windowPos.x + pos.x;
      windowPos.y = windowPos.y + pos.y;

      int offsetX = 30;
      int offsetY = 50;


      int squareSize = 30;
      
      for(int j = 0; j < row; j++) {
        int rowY = windowPos.y + j * squareSize + j * offsetY;
        if(rightToLeft) {
          for(int i = col - 1; i >= 0; i--) {
            ImColor ledColor = colors[j * col + i];  // 
            ImVec2 relativeLedPos1 = ImVec2(offsetX * i + windowPos.x + i * squareSize + offsetX * 4, rowY);
            drawList->AddRectFilled(relativeLedPos1, ImVec2(relativeLedPos1.x + squareSize, relativeLedPos1.y + squareSize), ledColor);
          }
        } else {
          for(int i = 0; i < col; i++) {
            ImColor ledColor = colors[j * col + i];  // 
            ImVec2 relativeLedPos1 = ImVec2(offsetX * i + windowPos.x + i * squareSize + offsetX * 4, rowY);
            drawList->AddRectFilled(relativeLedPos1, ImVec2(relativeLedPos1.x + squareSize, relativeLedPos1.y + squareSize), ledColor);
          }
        }
    }
}
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
  
  for(int i = 0; i < numLeds; i++) {
    _impl->colors.push_back(ImColor(0, 0, 0, 255));
  }
}



void NeopixelStrip::setColor(const int& ledIndex, const glm::vec4 &color) {
  // noop
  /*
    Update specific color of UI
  */
   _impl->colors[ledIndex] = ImColor(color.x, color.y, color.z, color.w);

}


void NeopixelStrip::show() {
  ImGui::Begin("Neopixel");
  
  auto* drawList = ImGui::GetWindowDrawList();
  int cols = _impl->numLeds / 7;
  
  int rows = _impl->numLeds / cols;
  ImVec2 windowPos = ImGui::GetWindowPos();
  
  int squareSize = 25;
  // decide on left orientation, top orientation, or right orientation
  switch(_impl->pin) {
    case 19:
      // left
      _impl->drawLedStrips(ImVec2(0, 80), 8, 2, true);
    case 13:
        // top
        _impl->drawLedStrips(ImVec2(135, 30), 1, 6, false);
      break;
     default:
      // right
      _impl->drawLedStrips(ImVec2(550, 80), 8, 2, true);
      break;
  }

  ImGui::End();
  
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

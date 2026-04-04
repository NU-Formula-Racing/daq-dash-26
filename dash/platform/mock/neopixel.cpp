#include "glm/ext/vector_float4.hpp"
#include <platform/interfaces.hpp>
#include <imgui.h>
#include <okay/okay.hpp>
#include <cstdint>
namespace dash {
    
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
            int ledIndex = i == 0 ? j : (numLeds - 1) - 1 * j;
            ImColor ledColor = colors[ledIndex];  
          
            ImVec2 relativeLedPos1 = ImVec2(offsetX * i + windowPos.x + i * squareSize + offsetX * 4, rowY);
            drawList->AddRectFilled(relativeLedPos1, ImVec2(relativeLedPos1.x + squareSize, relativeLedPos1.y + squareSize), ledColor);
          }
        } else {
          for(int i = 0; i < col; i++) {
            ImColor ledColor = colors[j * col + i];   
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
  /*
    Initialize boxes for each strip
    Initial color is black
  */
  
  _impl->pin = pin;
  _impl->numLeds = numLeds;
  
  for(int i = 0; i < numLeds; i++) {
    
    _impl->colors.push_back(ImColor(0, 255, 0, 255));
  }
}



void NeopixelStrip::setColor(const int& ledIndex, const glm::vec4 &color) {

   _impl->colors[ledIndex] = ImColor(color.x, color.y, color.z, color.w);
   
}


void NeopixelStrip::show() {

  ImGui::Begin("Neopixel");
  
  // decide on left orientation, top orientation, or right orientation
  switch(_impl->pin) {
    case 19:
      // left
      
      _impl->drawLedStrips(ImVec2(0, 80), 8, 2, true);
    case 13:
      // top
      _impl->drawLedStrips(ImVec2(150, 30), 1, 6, false);
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


};  // namespace dash
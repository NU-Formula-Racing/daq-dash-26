#include "glm/ext/vector_float4.hpp"

#include <okay/okay.hpp>

#include <cstdint>
#include <imgui.h>
#include <platform/interfaces.hpp>

namespace dash {

struct NeopixelStrip::NeopixelImpl {
    int pin;
    int numLeds;
    std::vector<ImColor> colors;

    void drawLedStrips(ImVec2 pos, int row, int col, bool rightToLeft) {
        if (row * col > numLeds) {
            okay::Engine.logger.error("Provided too many rows or columns.");
            return;
        }

        auto* drawList = ImGui::GetWindowDrawList();

        ImVec2 windowPos = ImGui::GetWindowPos();
        windowPos.x += pos.x;
        windowPos.y += pos.y;

        constexpr int offsetX = 5;
        constexpr int offsetY = 5;
        constexpr int squareSize = 20;
        constexpr float outlineThickness = 1.5f;

        const ImU32 outlineColor = IM_COL32(255, 255, 255, 255);

        for (int j = 0; j < row; j++) {
            int rowY = static_cast<int>(windowPos.y) + j * (squareSize + offsetY);

            for (int i = 0; i < col; i++) {
                int displayCol = rightToLeft ? (col - 1 - i) : i;
                int ledIndex = j * col + i;

                if (ledIndex < 0 || ledIndex >= static_cast<int>(colors.size())) {
                    continue;
                }

                ImColor ledColor = colors[ledIndex];

                ImVec2 p1 = ImVec2(
                    windowPos.x + displayCol * (squareSize + offsetX), static_cast<float>(rowY));

                ImVec2 p2 = ImVec2(p1.x + squareSize, p1.y + squareSize);

                drawList->AddRectFilled(p1, p2, ledColor);
                drawList->AddRect(p1, p2, outlineColor, 0.0f, 0, outlineThickness);
            }
        }
    }
};

NeopixelStrip::NeopixelStrip() : _impl(std::make_unique<NeopixelStrip::NeopixelImpl>()) {}
NeopixelStrip::~NeopixelStrip() {}

void NeopixelStrip::init(const int& pin, const int& numLeds) {
    _impl->pin = pin;
    _impl->numLeds = numLeds;

    _impl->colors.clear();
    _impl->colors.reserve(numLeds);

    for (int i = 0; i < numLeds; i++) {
        _impl->colors.push_back(ImColor(0, 255, 0, 255));
    }
}

void NeopixelStrip::setColor(const int& ledIndex, const glm::vec4& color) {
    _impl->colors[ledIndex] = ImColor(color.x, color.y, color.z, color.w);
}

void NeopixelStrip::show() {
    ImGui::Begin("Neopixel");

    constexpr float windowWidth = 315.0f;
    constexpr float windowHeight = 265.0f;

    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);

    // decide on left orientation, top orientation, or right orientation
    switch (_impl->pin) {
        case 19:
            // left
            _impl->drawLedStrips(ImVec2(10, 60), 8, 2, true);
        case 13:
            // top
            _impl->drawLedStrips(ImVec2(75, 30), 1, 7, false);
            break;
        default:
            // right
            _impl->drawLedStrips(ImVec2(260, 60), 8, 2, true);
            break;
    }

    ImGui::End();
}

void NeopixelStrip::cleanup() {
    // noop
}

};  // namespace dash

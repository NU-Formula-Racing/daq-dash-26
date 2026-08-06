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

    const int offsetX = 5;
    const int offsetY = 5;
    const int squareSize = 20;
    const float outlineThickness = 1.5f;

    void drawTopStrip(ImVec2 pos, int row, int col) {
        // colors go from left to right
        if (row * col > numLeds) {
            okay::Engine.logger.error("Provided too many rows or columns.");
            return;
        }

        auto* drawList = ImGui::GetWindowDrawList();

        ImVec2 windowPos = ImGui::GetWindowPos();
        windowPos.x += pos.x;
        windowPos.y += pos.y;

        const ImU32 outlineColor = IM_COL32(255, 255, 255, 255);

        for (int j = 0; j < row; j++) {
            int rowY = static_cast<int>(windowPos.y) + j * (squareSize + offsetY);

            for (int i = 0; i < col; i++) {
                int displayCol = (col - 1 - i);
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

    void drawLeftStrip(ImVec2 pos, int row, int col) {
        // two columns
        // 0 is top right
        // 7 is bottom right
        // 8 is top left
        // 15 is bottom left

        if (row * col > numLeds) {
            okay::Engine.logger.error("Provided too many rows or columns.");
            return;
        }

        auto* drawList = ImGui::GetWindowDrawList();

        ImVec2 windowPos = ImGui::GetWindowPos();
        windowPos.x += pos.x;
        windowPos.y += pos.y;

        const ImU32 outlineColor = IM_COL32(255, 255, 255, 255);

        for (int ledIndex = 0; ledIndex < row * col; ledIndex++) {
            if (ledIndex < 0 || ledIndex >= static_cast<int>(colors.size())) {
                continue;
            }

            int displayRow{};
            int displayCol{};

            if (ledIndex < row) {
                // Right column: top to bottom.
                displayRow = ledIndex;
                displayCol = 1;
            } else {
                // Left column: top to bottom.
                displayRow = ledIndex - row;
                displayCol = 0;
            }

            ImColor ledColor = colors[ledIndex];

            ImVec2 p1 = ImVec2(windowPos.x + displayCol * (squareSize + offsetX),
                windowPos.y + displayRow * (squareSize + offsetY));

            ImVec2 p2 = ImVec2(p1.x + squareSize, p1.y + squareSize);

            drawList->AddRectFilled(p1, p2, ledColor);
            drawList->AddRect(p1, p2, outlineColor, 0.0f, 0, outlineThickness);
        }
    }

    void drawRightStrip(ImVec2 pos, int row, int col) {
        // two columns
        // 0 is top left
        // 7 is bottom left
        // 8 is top right
        // 15 is bottom right

        if (row * col > numLeds) {
            okay::Engine.logger.error("Provided too many rows or columns.");
            return;
        }

        auto* drawList = ImGui::GetWindowDrawList();

        ImVec2 windowPos = ImGui::GetWindowPos();
        windowPos.x += pos.x;
        windowPos.y += pos.y;

        const ImU32 outlineColor = IM_COL32(255, 255, 255, 255);

        for (int ledIndex = 0; ledIndex < row * col; ledIndex++) {
            if (ledIndex < 0 || ledIndex >= static_cast<int>(colors.size())) {
                continue;
            }

            int displayRow{};
            int displayCol{};

            if (ledIndex < row) {
                // Left column: top to bottom.
                displayRow = ledIndex;
                displayCol = 0;
            } else {
                // Right column: top to bottom.
                displayRow = ledIndex - row;
                displayCol = 1;
            }

            ImColor ledColor = colors[ledIndex];

            ImVec2 p1 = ImVec2(windowPos.x + displayCol * (squareSize + offsetX),
                windowPos.y + displayRow * (squareSize + offsetY));

            ImVec2 p2 = ImVec2(p1.x + squareSize, p1.y + squareSize);

            drawList->AddRectFilled(p1, p2, ledColor);
            drawList->AddRect(p1, p2, outlineColor, 0.0f, 0, outlineThickness);
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
    if (ledIndex < 0 || ledIndex >= static_cast<int>(_impl->colors.size())) {
        return;
    }

    _impl->colors[ledIndex] = ImColor(color.x, color.y, color.z, color.w);
}

void NeopixelStrip::show() {
    return;
    constexpr float windowWidth = 315.0f;
    constexpr float windowHeight = 265.0f;

    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_Always);

    ImGui::Begin("Neopixel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);

    // decide on left orientation, top orientation, or right orientation
    switch (_impl->pin) {
        case 19:
            // left
            _impl->drawLeftStrip(ImVec2(10, 60), 8, 2);
            break;

        case 13:
            // top
            _impl->drawTopStrip(ImVec2(75, 30), 1, 7);
            break;

        default:
            // right
            _impl->drawRightStrip(ImVec2(260, 60), 8, 2);
            break;
    }

    ImGui::End();
}

void NeopixelStrip::cleanup() {
    // noop
}

};  // namespace dash

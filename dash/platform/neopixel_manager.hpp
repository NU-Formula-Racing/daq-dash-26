#ifndef __LIGHTS_HPP__
#define __LIGHTS_HPP__

#include <can/can_dbc.hpp>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <okay/okay.hpp>
#include <platform/interfaces.hpp>
#include <stdint.h>

namespace dash {

struct VirtualizedNeobar {
   public:
    VirtualizedNeobar() = default;

    VirtualizedNeobar(NeopixelStrip* strip, uint8_t numPixels, std::vector<uint8_t> mapping)
        : _strip(strip), _mapping(mapping), _numPixels(numPixels), _dirty(true) {
        for (int i = 0; i < numPixels; i++) {
            _currentColors.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }

    void setColor(uint8_t virtIdx, glm::vec4 color) {
        // get the existing color and see if they are diffent enough
        float ep = 0.001f;
        if (glm::epsilonEqual(_currentColors[virtIdx].x, color.x, ep) &&
            glm::epsilonEqual(_currentColors[virtIdx].y, color.y, ep) &&
            glm::epsilonEqual(_currentColors[virtIdx].z, color.z, ep) &&
            glm::epsilonEqual(_currentColors[virtIdx].w, color.w, ep)) {
            // okay::Engine.logger.debug("Color is the same");
            return;
        }
        _currentColors[virtIdx] = color;
        _dirty = true;
    }

    uint8_t numPixels() const { return _numPixels; }
    const std::vector<glm::vec4>& currentColors() const { return _currentColors; }
    uint8_t toHardwareIndex(uint8_t virtIdx) const { return _mapping[virtIdx]; }
    NeopixelStrip* strip() const { return _strip; }
    bool isDirty() const { return _dirty; }
    void clearDirty() { _dirty = false; }

   private:
    std::vector<uint8_t> _mapping;  // idx -> hwIdx
    std::vector<glm::vec4> _currentColors;
    NeopixelStrip* _strip;
    uint8_t _numPixels;
    bool _dirty{true};
};

class NeopixelManager : public okay::System<okay::SystemScope::GAME> {
   public:
    void initialize() {
        // create the strips
        _strips[0].init(19, 16);  // left
        _strips[1].init(13, 7);   // top
        _strips[2].init(18, 16);  // right

        // create the bars
        for (int i = 0; i < 5; i++) {
            _bars[i] = VirtualizedNeobar(
                &_strips[getHWIndexForBar(i)], numPixelsForBar(i), mappingAtBar(i));
        }

        // set all the bars to black
        glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < _bars[i].numPixels(); j++) {
                _bars[i].setColor(j, black);
            }
        }

        dbc::ecuDriveStatus::message.attach_rx_callback([this]() { onECUDriveStatus(); });
        startAnimation([this](float time) { idle(time); });
        updateDisplay();
    }

    void tick() {
        if (_animationFunction != nullptr) {
            float time = okay::Engine.time->timeSinceStartMs() - _animationStartTimeMs;
            _animationFunction(time);
        }

        updateDisplay();
    }

    void shutdown() {
        // make all the colors black
        glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < _bars[i].numPixels(); j++) {
                _bars[i].setColor(j, black);
            }
        }

        updateDisplay();

        // now cleanup
        for (int i = 0; i < 3; i++) {
            _strips[i].cleanup();
        }
    }

    void updateDisplay() {
        for (int i = 0; i < 3; i++) {
            // grab the relevant bars, and set the color on the strip
            if (i == 1) {
                continue;
            }

            for (int j = 0; j < 5; j++) {
                if (!_bars[j].isDirty())
                    continue;

                // this guy is broken
                if (j == 2)
                    continue;

                if (i != getHWIndexForBar(j))
                    continue;

                for (int k = 0; k < _bars[j].numPixels(); k++) {
                    _strips[i].setColor(_bars[j].toHardwareIndex(k), _bars[j].currentColors()[k]);
                }

                _bars[j].clearDirty();
            }

            _strips[i].show();
        }
    }

    VirtualizedNeobar& getBar(uint8_t barNum) { return _bars[barNum]; }

    void onECUDriveStatus() {
        uint8_t state = dbc::ecuDriveStatus::driveState->get();

        if (state == currentState)
            return;

        currentState = state;

        switch (state) {
            case 0:  // idle
                startAnimation([this](float time) { idle(time); });
                break;
            case 1:  // precharge
                startAnimation([this](float time) { precharge(time); });
                break;
            case 2:  // neutral
                startAnimation([this](float time) { neutral(time); });
                break;
            case 3:  // drive
                // do something
                startAnimation([this](float time) { drive(time); });
                break;
        }
    }

   private:
    std::array<VirtualizedNeobar, 5> _bars;
    std::array<NeopixelStrip, 3> _strips;
    uint32_t _animationStartTimeMs{0};
    std::function<void(float)> _animationFunction;
    uint8_t currentState{0};

    void startAnimation(std::function<void(float)> animationFunction) {
        _animationStartTimeMs = okay::Engine.time->timeSinceStartMs();
        _animationFunction = animationFunction;
    }

    uint8_t numPixelsForBar(uint8_t bar) {
        if (bar == 2) {
            return 7;
        } else {
            return 8;
        }
    }

    uint8_t getHWIndexForBar(uint8_t bar) {
        switch (bar) {
            case 0:
            case 1:
                return 0;
            case 2:
                return 1;
            case 3:
            case 4:
                return 2;
        }
        okay::Engine.logger.error("Invalid bar index {}", bar);
        return 0;
    }

    // master map
    std::vector<uint8_t> mappingAtBar(uint8_t bar) {
        static const std::vector<int> MASTER_MAP = {
            15, 14, 13, 12, 11, 10, 9, 8,  // bar 0
            7,  6,  5,  4,  3,  2,  1, 0,  // bar 1
            0,  1,  2,  3,  4,  5,  6,     // bar 2
            7,  6,  5,  4,  3,  2,  1, 0,  // bar 3
            15, 14, 13, 12, 11, 10, 9, 8,  // bar 4
        };

        std::vector<uint8_t> barMap;
        uint8_t num = numPixelsForBar(bar);

        // find starting point
        int offset = 0;
        for (int i = 0; i < bar; i++) {
            offset += numPixelsForBar(i);
        }

        for (int i = 0; i < num; i++)  // create the actual bars
        {
            int hwIdx = MASTER_MAP[offset + i];
            barMap.push_back((uint8_t)hwIdx);  // some funny mapping code
        }

        return barMap;
    }

    // ANIMATIONS

    static constexpr glm::vec4 colorFromHex(uint32_t hex) {
        float r = ((hex >> 16) & 0xFF) / 255.0f;
        float g = ((hex >> 8) & 0xFF) / 255.0f;
        float b = (hex & 0xFF) / 255.0f;
        return glm::vec4(r, g, b, 1.0f);
    }

    void idle(float time) {
        const float breathePeriod = 2000.0f;
        float brightness = (std::sin(time / breathePeriod) + 1.0f) / 2.0f;  // +1 for normalize
        glm::vec4 purple = colorFromHex(0x4E2A84) * abs(brightness);
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < getBar(i).numPixels(); j++) {
                getBar(i).setColor(j, purple);
            }
        }
    }

    void neutral(float time) {
        static std::vector<glm::vec4> palette = {colorFromHex(0x4E2A84),
                                                 colorFromHex(0x4E2A84),
                                                 colorFromHex(0x00000),
                                                 colorFromHex(0x00000)};
        const float moveSpeed = 1 / 200.0f;

        for (int i = 0; i < 5; i++) {  // for all 5 bars
            int barOffset = i * 2;
            for (int j = 0; j < getBar(i).numPixels(); j++) {  // this indexes the leds on each bar
                int colorIndex = static_cast<int>(time * moveSpeed + j + barOffset);
                glm::vec4 color = palette[colorIndex % palette.size()];
                int invert = getBar(i).numPixels() - j - 1;
                getBar(i).setColor(invert, color);
            }
        }
    }

    void precharge(float time) {
        glm::vec4 yellow = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

        float prechargePercentage =
            static_cast<float>(dbc::rearInverterMotorStatus::dcVoltage->get()) /
            static_cast<float>(dbc::bmsSoe::batteryVoltage->get());
        for (int i = 0; i < 5; i++) {
            // probably something here
            float t = 0.9f / getBar(i).numPixels();
            for (int j = 0; j < getBar(i).numPixels(); j++) {
                if (prechargePercentage >= t * j) {
                    getBar(i).setColor(j, yellow);
                } else {
                    getBar(i).setColor(j, black);
                }
            }
        }
    }

    void drive(float time) {
        const float blinkTime = 500;
        const int numBlinks = 3;

        if (time < blinkTime * 2 * numBlinks) {
            // we are still blinking
            float brightness = static_cast<int>(floor(time / blinkTime)) % 2;
            glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            color *= brightness;
            // set the colors
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < getBar(i).numPixels(); j++) {
                    getBar(i).setColor(j, color);
                }
            }
        } else {
            // we are now in throttle light mode
            const float appsMax = 100;
            float throttlePercentage =
                static_cast<float>(dbc::ecuThrottle::apps1Throttle->get()) / appsMax;
            glm::vec4 botColor = colorFromHex(0x00FF00);
            glm::vec4 topColor = colorFromHex(0xFFDD00);

            for (int i = 0; i < 5; i++) {
                int numPixels = getBar(i).numPixels();
                int numFull = static_cast<int>(floor(throttlePercentage * numPixels));

                for (int j = 0; j < numFull; j++) {
                    glm::vec4 color = glm::mix(
                        botColor, topColor, static_cast<float>(j) / static_cast<float>(numPixels));
                    getBar(i).setColor(j, color);
                }

                // turn off the rest of the pixels
                for (int j = numFull; j < numPixels; j++) {
                    if (j == numFull) {
                        glm::vec4 color =
                            glm::mix(botColor,
                                     topColor,
                                     static_cast<float>(j) / static_cast<float>(numPixels));
                        // set it to partial brightness to make a smoother transition
                        glm::vec4 partialColor = color * (throttlePercentage * numPixels - numFull);
                        getBar(i).setColor(j, partialColor);
                        continue;
                    }

                    getBar(i).setColor(j, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
                }
            }
        }
    }
};

}  // namespace dash

#endif
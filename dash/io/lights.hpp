#ifndef __LIGHTS_HPP__
#define __LIGHTS_HPP__

#include <can/can_dbc.hpp>
#include <platform/platform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <okay/core/okay.hpp>
#include <stdint.h>
#include <cstdint>
#include <vector>
#include "glm/ext/vector_float4.hpp"
#include "okay/core/system/okay_system.hpp"

namespace dash {

struct VirtualizedNeobar {
   public:
    VirtualizedNeobar() = default;

    VirtualizedNeobar(platform::NeopixelStrip* strip,
                      uint8_t numPixels,
                      std::vector<uint8_t> mapping)
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

    uint8_t numPixels() const {
        return _numPixels;
    }
    const std::vector<glm::vec4>& currentColors() const {
        return _currentColors;
    }
    uint8_t toHardwareIndex(uint8_t virtIdx) const {
        return _mapping[virtIdx];
    }
    platform::NeopixelStrip* strip() const {
        return _strip;
    }
    bool isDirty() const {
        return _dirty;
    }
    void clearDirty() {
        _dirty = false;
    }

   private:
    std::vector<uint8_t> _mapping;  // idx -> hwIdx
    std::vector<glm::vec4> _currentColors;
    platform::NeopixelStrip* _strip;
    uint8_t _numPixels;
    bool _dirty{true};
};

class NeopixelManager : public okay::OkaySystem<okay::OkaySystemScope::GAME> {
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

        updateDisplay();
    }

    void tick() {
        if (_animationFunction != nullptr) {
            _animationFunction();
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
            _strips[i].show();
            for (int j = 0; j < 5; j++) {
                if (!_bars[j].isDirty())
                    continue;

                if (i != getHWIndexForBar(j))
                    continue;

                for (int k = 0; k < _bars[j].numPixels(); k++) {
                    _strips[i].setColor(_bars[j].toHardwareIndex(k), _bars[j].currentColors()[k]);
                }

                _bars[j].clearDirty();
            }

            if (i == 1)
                continue;

            _strips[i].show();
        }
    }

    VirtualizedNeobar& getBar(uint8_t barNum) {
        return _bars[barNum];
    }

    void onECUDriveStatus() {
        uint8_t state = dbc::ecuDriveStatus::driveState->get();

        if (state == currentState)
            return;

        currentState = state;

        switch (state) {
            case 0:  // idle
                startAnimation([this]() { idle(); });
                break;
            case 1:  // precharge
                startAnimation([this]() { precharge(); });
                break;
            case 2:  // neutral
                startAnimation([this]() { neutral(); });
                break;
            case 3:  // drive
                // do something
                startAnimation([this]() { drive(); });
                break;
        }
    }

   private:
    std::array<VirtualizedNeobar, 5> _bars;
    std::array<platform::NeopixelStrip, 3> _strips;
    uint32_t _animationStartTimeMs{0};
    std::function<void()> _animationFunction;
    uint8_t currentState{0};

    void startAnimation(std::function<void()> animationFunction) {
        _animationStartTimeMs =
            std::chrono::steady_clock::now().time_since_epoch().count() / 1000.0f;
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

    void idle() {
        const float breathePeriod = 2.0;  // in seconds
        // breathing timing; uses steady clock.
        float nowMs = std::chrono::steady_clock::now().time_since_epoch().count() / 1000.0f;
        float time = (nowMs - _animationStartTimeMs) / 1000.0f;
        // in seconds
        float brightness = (std::sin(time * breathePeriod) + 1.0f) / 2.0f;  // +1 for normalize
        // define purple
        glm::vec4 purple(1.0f, 0.0f, 1.0f, brightness);
        for (int i = 0; i < 5; i++) {                          // for all 5 bars
            for (int j = 0; j < getBar(i).numPixels(); j++) {  // this indexes the leds on each bar
                getBar(i).setColor(j, purple);
            }
        }
    }

    void neutral() {
        static std::vector<glm::vec4> palette = {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                                                 glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                                                 glm::vec4(1.0f, 0.5f, 0.0f, 1.0f),
                                                 glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
                                                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                                                 glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                                                 glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
                                                 glm::vec4(1.0f, 0.0f, 0.5f, 1.0f)};

        float nowMS = std::chrono::steady_clock::now().time_since_epoch().count() / 1000.0f;
        float time = (nowMS - _animationStartTimeMs) / 1000.0f;
        const float moveSpeed = 15.0f;

        for (int i = 0; i < 5; i++) {  // for all 5 bars
            int barOffset = i * 3;
            for (int j = 0; j < getBar(i).numPixels(); j++) {  // this indexes the leds on each bar
                int colorIndex = static_cast<int>(time * moveSpeed + j + barOffset);
                glm::vec4 color = palette[colorIndex % palette.size()];
                getBar(i).setColor(j, color);
            }
        }
    }

    void precharge() {
        float nowMS = std::chrono::steady_clock::now().time_since_epoch().count() / 1000.0f;
        float time = (nowMS - _animationStartTimeMs) / 1000.0f;
        glm::vec4 yellow = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
        float prechargePercentage = static_cast<float>(dbc::rearInverterMotorStatus::dcVoltage->get())/static_cast<float>(dbc::bmsSoe::batteryVoltage->get());
        for(int i = 0; i < 5; i++){
            //probably something here
            float t = 0.9f/getBar(i).numPixels();
            for(int j = 0; j < getBar(i).numPixels(); j++){

                if(prechargePercentage >= t*i){
                    getBar(i).setColor(j, yellow);
                }
                else{
                    getBar(i).setColor(j, black);
                }

            }
        }

    }

    void drive() {
        float nowMS = std::chrono::steady_clock::now().time_since_epoch().count() / 1000.0f;
        float time = (nowMS - _animationStartTimeMs) / 1000.0f;

        const float blinkTime = 1000;
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
            const int16_t appsMax = 4000;
            float throttlePercentage = dbc::ecuThrottle::apps1Throttle->get() / appsMax;

            float partialBrightness = fmodf(throttlePercentage * 8.0f, 1.0f);
            glm::vec4 blue = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            glm::vec4 orange = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

            for (int i = 0; i < 5; i++) {
                if (i == 2)
                    continue;

                for (int j = 0; j < floor(throttlePercentage * 8.0f); j++) {
                    glm::vec4 color = glm::mix(blue, orange, static_cast<float>(i / 8));
                    getBar(i).setColor(j, color);
                }
                // getBar(i).setColor(floor(throttlePercentage * 8) + 1, color * partialBrightness);
            }
        }
    }
};

} // namespace dash

#endif  // __LIGHTS_HPP__
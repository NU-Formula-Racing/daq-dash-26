#ifndef __LIGHTS_HPP__
#define __LIGHTS_HPP__

#include <platform/platform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <okay/core/okay.hpp>
#include <stdint.h>
#include <cstdint>
#include <vector>
#include "okay/core/system/okay_system.hpp"

namespace dash {

struct VirtualizedNeobar {
   public:
    VirtualizedNeobar() = default;

    VirtualizedNeobar(platform::NeopixelStrip* strip,
                      uint8_t numPixels,
                      std::vector<uint8_t> mapping)
        : _strip(strip), _mapping(mapping), _numPixels(numPixels) {
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

        _strip->setColor(_mapping[virtIdx], color);
        _currentColors[virtIdx] = color;
        _dirty = true;
    }

    void show() {
        if (!_dirty) {
            okay::Engine.logger.debug("Skipping show call");
            return;
        }
        _strip->show();
        _dirty = false;
    }

    uint8_t numPixels() {
        return _numPixels;
    }

   private:
    std::vector<uint8_t> _mapping;  // idx -> hwIdx
    std::vector<glm::vec4> _currentColors;
    platform::NeopixelStrip* _strip;
    uint8_t _numPixels;
    bool _dirty{false};
};

class NeopixelDisplay : public okay::OkaySystem<okay::OkaySystemScope::GAME> {
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
    }

    VirtualizedNeobar& getBar(uint8_t barNum) {
        return _bars[barNum];
    }

    void shutdown() {
        // make all the colors black
        glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < _bars[i].numPixels(); j++) {
                _bars[i].setColor(j, black);
            }
        }

        for (int i = 0; i < 3; i++) {
            _strips[i].show();
        }

        // now cleanup
        for (int i = 0; i < 3; i++) {
            _strips[i].cleanup();
        }
    }

   private:
    std::array<VirtualizedNeobar, 5> _bars;
    std::array<platform::NeopixelStrip, 3> _strips;

    uint8_t numPixelsForBar(uint8_t bar) {
        // bars 0, 1, 3, 4 have 8
        // bar 2 has 7
        if (bar == 2) {
            return 7;
        } else {
            return 8;
        }
    }

    uint8_t getHWIndexForBar(uint8_t bar) {
        // bar 0, 1 are on strip 0
        // bar 2 is on strip 1
        // bar 3, 4 are on strip 2
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
};

}  // namespace dash

#endif  // __LIGHTS_HPP__
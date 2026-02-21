#ifndef __LIGHTS_HPP__
#define __LIGHTS_HPP__

#include <platform/platform.hpp>
#include <glm/glm.hpp>
#include <stdint.h>
#include <cstdint>
#include <vector>

namespace dash {

struct VirtualizedNeobar {
   public:
    VirtualizedNeobar() = default;

    VirtualizedNeobar(platform::NeopixelStrip* strip,
                      uint8_t numPixels,
                      std::vector<uint8_t> mapping)
        : _strip(strip), _mapping(mapping), _numPixels(numPixels) {
    }

    void setColor(uint8_t virtIdx, glm::vec4 color) {
        _strip->setColor(_mapping[virtIdx], color);
    }

    void show() {
        _strip->show();
    }

    uint8_t numPixels() {
        return _numPixels;
    }

   private:
    std::vector<uint8_t> _mapping;  // idx -> hwIdx
    platform::NeopixelStrip* _strip;
    uint8_t _numPixels;
};

class NeopixelDisplay {
   public:
    // defines shape for project
    uint8_t numPixelsAtBar(uint8_t bar) {
        // bars 0, 1, 3, 4 have 8
        // bar 2 has 7
        if (bar == 2) {
            return 7;
        } else {
            return 8;
        }
    }

    uint8_t getHWStrpIndexForBar(uint8_t bar) {
        if (bar < 2) {
            return 0;
        } else if (bar == 2) {
            return 1;
        } else {
            return 2;
        }
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
        uint8_t num = numPixelsAtBar(bar);

        // find starting point
        int offset = 0;
        for (int i = 0; i < bar; i++) {
            offset += numPixelsAtBar(i);
        }

        for (int i = 0; i < num; i++)  // create the actual bars
        {
            int hwIdx = MASTER_MAP[offset + i];
            barMap.push_back((uint8_t)hwIdx);  // some funny mapping code
        }

        return barMap;
    }

    void initialize() {
        // create the strips
        _strips[0].init(19, 16);  // left
        _strips[1].init(13, 7);   // top
        _strips[2].init(18, 16);  // right

        // create the bars
        for (int i = 0; i < 5; i++) {
            _bars[i] = VirtualizedNeobar(
                &_strips[getHWStrpIndexForBar(i)], numPixelsAtBar(i), mappingAtBar(i));
        }
    }

    VirtualizedNeobar& getBar(uint8_t barNum) {
        return _bars[barNum];
    }

   private:
    std::array<VirtualizedNeobar, 5> _bars;
    std::array<platform::NeopixelStrip, 3> _strips;
};

}  // namespace dash

#endif  // __LIGHTS_HPP__
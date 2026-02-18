#ifndef __LIGHTS_HPP__
#define __LIGHTS_HPP__

#include <neo-pixel/ws2812-rpi.h>
#include <stdint.h>
#include <cstdint>
#include <vector>

// class/struct names are PascalCase
// public member variables are camelCase
// function names are camelCase
// private memberVariables are _camelCase with a prefix 

struct Color {
    uint8_t r, g, b, a;
};

struct VirtualizedNeobar {
   public:

    VirtualizedNeobar(uint8_t numPixels, std::vector<uint8_t> mapping) : 
        _strip(numPixels),
        _mapping(mapping)
        {}

    void setColor(uint8_t virtIdx, Color c)
    {
        _strip.setPixelColor(_mapping[virtIdx], c.r, c.g, c.b);
    }

   private:
    std::vector<uint8_t> _mapping;  // idx -> hwIdx
    NeoPixel _strip;
};

class NeopixelDisplay {

    uint8_t numPixelsAtBar(uint8_t bar) {
        // bars 0, 1, 3, 4 have 8
        // bar 2 has 7

        return 8;
    }

    std::vector<uint8_t> mappingAtBar(uint8_t bar) {
        // todo, more complicated algorithm
        return std::vector<uint8_t>();
    }


    void initialize() {
        // create the bars
        for (int i = 0; i < 5; i++) {
            bars[i] = VirtualizedNeobar(numPixelsAtBar(i), mappingAtBar(i));
        }
    }

    VirtualizedNeobar &getBar(uint8_t barNum) {
        return bars[barNum];
    }

    private:
    std::array<VirtualizedNeobar, 5> bars;
};

}

#endif  // __LIGHTS_HPP__
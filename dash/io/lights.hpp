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
    public:
    // defines shape for project
    uint8_t numPixelsAtBar(uint8_t bar) {
        // bars 0, 1, 3, 4 have 8
        // bar 2 has 7
        if (bar == 2) {
            return 7;
            }
        else {
            return 8;
            }
    }

    // master map
    std::vector<uint8_t> mappingAtBar(uint8_t bar) {
        // todo, more complicated algorithm
        static const std::vector<int> MASTER_MAP =
        {
            8, 9, 10, 11, 12, 13, 14, 15, 
            7, 6, 5, 4, 3, 2, 1, 0,
            0, 1, 2, 3, 4, 5, 6,
            7, 6, 5, 4, 3, 2, 1, 0,
            8, 9, 10, 11, 12, 13, 14, 15
        };

        std::vector<uint8_t> barMap;
        uint8_t num = numPixelsAtBar(bar);

        // find starting point
        int offset = 0; 
        for (int i = 0; i < bar; i++) // counts up for the number of bars, shows where each bar starts ("offset")
        {
            offset += numPixelsAtBar(i);
        }

        for (int i = 0; i < num; i++) // create the actual bars
            {
                int hwIdx = MASTER_MAP[offset + i]; 
                barMap.push_back((uint8_t)hwIdx); //some funny mapping code
            }

        return barMap; // can you just index these like Neobar[1][0] is bottom led of middle left bar
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

#endif  // __LIGHTS_HPP__
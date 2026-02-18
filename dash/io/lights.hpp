#ifdef __LIGHTS_HPP__
#define __LIGHTS_HPP__

#include <dash/drivers/neo-pixel/ws2812-rpi.h>

// class/struct names are PascalCase
// public member variables are camelCase
// function names are camelCase
// private memberVariables are _camelCase with a prefix _


struct VirtualizedNeobar
{
    public:
        void setColor(uint8_t virtIdx, Color c); 
            {
            _strip.set_color(_mapping[virtIdx], c.r, c.g, c.b);
            _strip.set_brightness(mapping[virtIdx], c.a);
            }
    private:
        std::vector<uint8_t> _mapping; // idx -> hwIdx
        Neopixel_strip _strip;
};

struct Bar{
    vector<Color> l;
};

class Neopixels {
    array<Bar, 5> bars;

};

class NeopixelDisplay
{
    static constexpr std::array <std::vector<uint8_t>;s> MAP.LUT = 
    {
        vector<int> mapping = {8, 9, 10, 11, 12, 13, 14, 15, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, -1, 7, 6, 5, 4, 3, 2, 1, 0, 8, 9, 10, 11, 12, 13, 14, 15};

    }
    
    std::array<VirtualizedNeobar>_bars;

    void tickLights()
        uint32_t animStart = anim.start;
        uint32_t animEnd = anim.end;
        uint32_t now = millis();

        float t = (float)(now - animStart) / (float)(animEnd - animStart);
        
        if (t > 1.0f) 
        {
            t = 1.0f;
        }

        anim.update(t);

        // is animation active
        if (t >= 1.0f) 
        {
            anim.active = false;
        }
}
}


#endif // __LIGHTS_HPP__
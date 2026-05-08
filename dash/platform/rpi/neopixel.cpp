#include "platform/rpi/gpio_manager.hpp"

#include <okay/okay.hpp>

#include <cstdint>
#include <drivers/neopixel/ws2811.h>
#include <gpiod.hpp>
#include <memory>
#include <platform/interfaces.hpp>

extern "C" {
#include <drivers/neopixel/gpio.h>
#include <drivers/neopixel/mailbox.h>  // mapmem(), unmapmem(), DEV_GPIOMEM
}

namespace dash {

// We use GPIO13, 19 (up, left)
// and GPIO 18 (right)
#define GPIO_L 19
#define GPIO_U 13
#define GPIO_R 18
#define MAX_LEDS 16

// For muxing
#define EN_L 27
#define EN_U 17

#define TARGET_FREQ 800000
#define DMA 10
#define STRIP_TYPE WS2811_STRIP_GBR  // WS2812/SK6812RGB integrated chip+leds

uint8_t gamma8[] = {0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    4,
    4,
    4,
    4,
    4,
    5,
    5,
    5,
    5,
    6,
    6,
    6,
    6,
    7,
    7,
    7,
    7,
    8,
    8,
    8,
    9,
    9,
    9,
    10,
    10,
    10,
    11,
    11,
    11,
    12,
    12,
    13,
    13,
    13,
    14,
    14,
    15,
    15,
    16,
    16,
    17,
    17,
    18,
    18,
    19,
    19,
    20,
    20,
    21,
    21,
    22,
    22,
    23,
    24,
    24,
    25,
    25,
    26,
    27,
    27,
    28,
    29,
    29,
    30,
    31,
    32,
    32,
    33,
    34,
    35,
    35,
    36,
    37,
    38,
    39,
    39,
    40,
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    48,
    49,
    50,
    50,
    51,
    52,
    54,
    55,
    56,
    57,
    58,
    59,
    60,
    61,
    62,
    63,
    64,
    66,
    67,
    68,
    69,
    70,
    72,
    73,
    74,
    75,
    77,
    78,
    79,
    81,
    82,
    83,
    85,
    86,
    87,
    89,
    90,
    92,
    93,
    95,
    96,
    98,
    99,
    101,
    102,
    104,
    105,
    107,
    109,
    110,
    112,
    114,
    115,
    117,
    119,
    120,
    122,
    124,
    126,
    127,
    129,
    131,
    133,
    135,
    137,
    138,
    140,
    142,
    144,
    146,
    148,
    150,
    152,
    154,
    156,
    158,
    160,
    162,
    164,
    167,
    169,
    171,
    173,
    175,
    177,
    180,
    182,
    184,
    186,
    189,
    191,
    193,
    196,
    198,
    200,
    203,
    205,
    208,
    210,
    213,
    215,
    218,
    220,
    223,
    225,
    228,
    231,
    233,
    236,
    239,
    241,
    244,
    247,
    249,
    252,
    255};

static ws2811_t s_ledString = {
    .freq = TARGET_FREQ,
    .dmanum = DMA,
    .channel =
        {
            [0] =
                {
                    .gpionum = GPIO_R,
                    .invert = 0,
                    .count = MAX_LEDS,
                    .strip_type = STRIP_TYPE,
                    .brightness = 255,
                    .gamma = gamma8,
                },
            // the first is up/left
            [1] =
                {
                    .gpionum = GPIO_L,
                    .invert = 0,
                    .count = MAX_LEDS,
                    .strip_type = STRIP_TYPE,
                    .brightness = 255,
                    .gamma = gamma8,

                },
        },
};

static bool s_hasInitialized{false};
static bool s_hasCleanedUp{false};

static uint32_t encodeToWWRRGGBB(glm::vec4 color) {
    color = color * color.a;
    return static_cast<uint32_t>(color.w * 255) << 24 | static_cast<uint32_t>(color.z * 255) << 16 |
           static_cast<uint32_t>(color.y * 255) << 8 | static_cast<uint32_t>(color.x * 255);
}

struct NeopixelStrip::NeopixelImpl {
    int pin;
    int numLeds;
    int channel;
};

NeopixelStrip::NeopixelStrip() : _impl(std::make_unique<NeopixelStrip::NeopixelImpl>()) {}
NeopixelStrip::~NeopixelStrip() {}

void NeopixelStrip::init(const int& pin, const int& numLeds) {
    _impl->pin = pin;
    _impl->numLeds = numLeds;

    if (pin == GPIO_L || pin == GPIO_U) {
        _impl->channel = 1;
    } else if (pin == GPIO_R) {
        _impl->channel = 0;
    } else {
        _impl->channel = -1;  // warning
    }

    if (_impl->channel == -1) {
        okay::Engine.logger.error("Unable to find channel for pin {}", _impl->pin);
    }

    if (s_hasInitialized)
        return;

    ws2811_return_t code = ws2811_init(&s_ledString);

    if (code != WS2811_SUCCESS) {
        okay::Engine.logger.error(
            "Unable to initialize neopixels : {}", ws2811_get_return_t_str(code));
    }

    gpiod::line_settings outputSettings;
    outputSettings.set_direction(gpiod::line::direction::OUTPUT);
    outputSettings.set_output_value(gpiod::line::value::INACTIVE);
    GPIOManager::instance().registerPin(EN_L, outputSettings);
    GPIOManager::instance().registerPin(EN_U, outputSettings);

    s_hasInitialized = true;
}

void NeopixelStrip::setColor(const int& ledIndex, const glm::vec4& color) {
    if (_impl->channel == -1) {
        okay::Engine.logger.error("Channel is -1 for pin {}", _impl->pin);
        return;
    }

    if (ledIndex >= _impl->numLeds) {
        okay::Engine.logger.error("Led index {} is out of range", ledIndex);
        return;
    }

    ws2811_channel_t* channel = &(s_ledString.channel[_impl->channel]);
    channel->leds[ledIndex] = encodeToWWRRGGBB(color);
}

void NeopixelStrip::show() {
    ws2811_wait(&s_ledString);
    ws2811_channel_t* channel = &(s_ledString.channel[_impl->channel]);

    if (_impl->pin == GPIO_L) {
        okay::Engine.logger.debug("Enabling Left");
        GPIOManager::instance().gpioWritePin(EN_L, GpioLevel::G_HIGH);
        GPIOManager::instance().gpioWritePin(EN_U, GpioLevel::G_LOW);
    } else if (_impl->pin == GPIO_U) {
        // set EN_U high and EN_L low -> go to upper side
        okay::Engine.logger.debug("Enabling Up");
        GPIOManager::instance().gpioWritePin(EN_U, GpioLevel::G_HIGH);
        GPIOManager::instance().gpioWritePin(EN_L, GpioLevel::G_LOW);
    }

    if (channel->gpionum != _impl->pin) {  // Be better to check if the thing is greater than
        // Capture old pin + base BEFORE fini
        const int oldPin = channel->gpionum;
        const int newPin = _impl->pin;
        channel->gpionum = newPin;
        channel->count = MAX_LEDS;
    }

    channel->count = _impl->numLeds;
    ws2811_render(&s_ledString);
    ws2811_wait(&s_ledString);
}

void NeopixelStrip::cleanup() {
    if (s_hasCleanedUp)
        return;

    s_hasCleanedUp = true;
    s_hasInitialized = false;
    ws2811_fini(&s_ledString);
}

}  // namespace dash

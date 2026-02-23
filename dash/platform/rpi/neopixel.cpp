#include <cstdint>
#include <memory>
#include <platform/platform.hpp>
#include <drivers/neopixel/ws2811.h>
#include <okay/core/okay.hpp>

extern "C" {
#include <drivers/neopixel/gpio.h>
#include <drivers/neopixel/mailbox.h>  // mapmem(), unmapmem(), DEV_GPIOMEM
}

namespace dash::platform {

// We use GPIO13, 19 (up, left)
// and GPIO 18 (right)
#define GPIO_L 19
#define GPIO_U 13
#define GPIO_R 18
#define MAX_LEDS 16

#define TARGET_FREQ 800000
#define DMA 10
#define STRIP_TYPE WS2811_STRIP_GBR  // WS2812/SK6812RGB integrated chip+leds

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
                },
            // the first is up/left
            [1] =
                {
                    .gpionum = GPIO_L,
                    .invert = 0,
                    .count = MAX_LEDS,
                    .strip_type = STRIP_TYPE,
                    .brightness = 255,
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

NeopixelStrip::NeopixelStrip() : _impl(std::make_unique<NeopixelStrip::NeopixelImpl>()) {
}
NeopixelStrip::~NeopixelStrip() {
}

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
        okay::Engine.logger.error("Unable to initialize neopixels : {}",
                                  ws2811_get_return_t_str(code));
    }

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
    if (channel->gpionum != _impl->pin) {
        // Capture old pin + base BEFORE fini
        const int oldPin = channel->gpionum;
        const int newPin = _impl->pin;
        const uint32_t perBase = s_ledString.rpi_hw->periph_base;

        volatile gpio_t* gpio =
            (volatile gpio_t*)mapmem(GPIO_OFFSET + perBase, sizeof(gpio_t), DEV_GPIOMEM);
        if (gpio) {
            gpio_output_set(gpio, oldPin, 0);
            gpio_output_set(gpio, newPin, 0);

            gpio_output_set(gpio, oldPin, 1);
            gpio_level_set(gpio, oldPin, 0);

            usleep(50);
            // enable ONLY the selected pin for PWM channel 1
            int alt = pwm_pin_alt(_impl->channel, newPin);
            if (alt >= 0) {
                gpio_function_set(gpio, newPin, alt);
            }
        } else {
            okay::Engine.logger.error("Unable to map gpio memory");
            // while (true) {};
        }

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

}  // namespace dash::platform
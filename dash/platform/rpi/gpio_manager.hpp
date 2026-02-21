#include <platform/platform.hpp>

#include <gpiod.hpp>

#include <cstdint>
#include <unordered_map>

namespace dash::platform {

class GPIOManager {
    static GPIOManager& instance();

    void registerPin(uint8_t offset, gpiod::line_settings settings);
    void releasePin(uint8_t offset);
    void registerInterrupt(uint8_t offset, gpiod::line_settings settings, std::function<void()> callback);

    void start();

    bool gpioWritePin(uint8_t offset, GpioLevel level);
    bool gpioReadPin(uint8_t offset, GpioLevel& out);

    void tick();

private:
    GPIOManager();
    GPIOManager(const GPIOManager&) = delete;
    GPIOManager& operator=(const GPIOManager&) = delete;
    ~GPIOManager();

    std::unordered_map<uint8_t, std::function<void()>> _callbacks;
    std::unordered_map<uint8_t, gpiod::line_settings> _settings;

    gpiod::chip _chip;
    gpiod::line_request _request;
};

} // namespace dash::platform

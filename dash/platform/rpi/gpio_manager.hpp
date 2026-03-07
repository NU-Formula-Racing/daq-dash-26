#include <platform/platform.hpp>

#include <gpiod.hpp>

#include <cstdint>
#include <unordered_map>

namespace dash::platform {

class GPIOManager {
public:
    static GPIOManager& instance();

    bool registerPin(uint8_t offset, gpiod::line_settings settings);
    void releasePin(uint8_t offset);
    void registerInterrupt(uint8_t offset, gpiod::line_settings settings, std::function<void()> callback, EdgeType edge);

    void start();

    bool gpioWritePin(uint8_t offset, GpioLevel level);
    bool gpioReadPin(uint8_t offset, GpioLevel& out);

    void tick();

private:
    GPIOManager();
    GPIOManager(const GPIOManager&) = delete;
    GPIOManager& operator=(const GPIOManager&) = delete;

    std::unordered_map<uint8_t, std::function<void()>> _risingCallbacks;
    std::unordered_map<uint8_t, std::function<void()>> _fallingCallbacks;
    std::unordered_map<uint8_t, gpiod::line_settings> _settings;

    std::unique_ptr<gpiod::chip> _chip;
    std::unique_ptr<gpiod::line_request> _request;

    bool _started = false;
};

} // namespace dash::platform

#include <platform/platform.hpp>
#include <platform/rpi/gpio_manager.hpp>
#include "drivers/can/include/nfr_can/MCP2515.hpp"
#include <okay/core/okay.hpp>
#include "can/can_dbc.hpp"

namespace dash::platform {

void tick(){
    GPIOManager::instance().tick();
}

inline void configureCANDrivers(dash::platform::SPI &spi, dash::platform::GPIO &gpio, dash::platform::Clock &clock) {
    dbc::driveBus.set_driver(std::make_unique<MCP2515>(spi, gpio, clock));

    // check for errors
    if (gpio.checkError()) {
        okay::Engine.logger.error("Failed to initialize GPIO");
    }
}

} // namespace dash::platform

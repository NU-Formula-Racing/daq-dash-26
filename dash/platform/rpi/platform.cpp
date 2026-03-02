#include <platform/platform.hpp>
#include <platform/rpi/gpio_manager.hpp>
#include "drivers/can/include/nfr_can/MCP2515.hpp"
#include <okay/core/okay.hpp>
#include <can/can_dbc.hpp>
#include "okay/core/util/option.hpp"

namespace dash::platform {

void preUpdate() {}

void postUpdate() {
    GPIOManager::instance().tick();
}

static SPI s_canSpi;
static GPIO s_canGPIO{0, true};
static Clock s_canClock;

void configureCANDriver() {
    dbc::driveBus.set_driver(std::make_unique<MCP2515>(s_canSpi, s_canGPIO, s_canClock));

    // check for errors
    if (s_canGPIO.checkError()) {
        okay::Engine.logger.error("Failed to initialize GPIO");
    }
}

} // namespace dash::platform

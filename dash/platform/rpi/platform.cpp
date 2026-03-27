#include <nfr_can/MCP2515.hpp>
#include <platform/input_manager.hpp>
#include <platform/interfaces.hpp>
#include <platform/platform.hpp>
#include <platform/rpi/gpio_manager.hpp>


namespace dash {

void Platform::tick() {
    GPIOManager::instance().tick();
    InputManager::instance().tick();
}

static SPI s_canSpi;
static GPIO s_canGPIO{0, true};
static Clock s_canClock;

void Platform::configureCANDriver(CAN_Bus& bus) {
    bus.set_driver(std::make_unique<MCP2515>(s_canSpi, s_canGPIO, s_canClock));

    // check for errors
    if (s_canGPIO.checkError()) {
        okay::Engine.logger.error("Failed to initialize GPIO");
    }
}

}  // namespace dash

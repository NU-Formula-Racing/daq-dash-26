#include <can/can_dbc.hpp>
#include <nfr_can/CAN_interface.hpp>
#include <nfr_can/MCP2515.hpp>
#include <platform/can.hpp>
#include <platform/input_manager.hpp>
#include <platform/interfaces.hpp>
#include <platform/rpi/gpio_manager.hpp>

#include <chrono>
#include <thread>

namespace dash {

static SPI s_canSpi;
static GPIO s_canGPIO{0, true};
static Clock s_canClock;

void CANManager::initialize() {
    dbc::driveBus.set_driver(std::make_unique<MCP2515>(s_canSpi, s_canGPIO, s_canClock));

    // check for errors
    if (s_canGPIO.checkError()) {
        okay::Engine.logger.error("Failed to initialize GPIO");
    }

    BaudRate baud500k = BaudRate::kBaud500K;
    if (!dbc::driveBus.init(baud500k)) {
        okay::Engine.logger.error("Failed to initialize CAN bus");

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}

void CANManager::tick() {
    GPIOManager::instance().tick();
    InputManager::instance().tick();
    dbc::driveBus.tick_bus();
}

}  // namespace dash

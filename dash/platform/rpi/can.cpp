#include <can/can_dbc.hpp>
#include <chrono>
#include <nfr_can/CAN_interface.hpp>
#include <nfr_can/MCP2515.hpp>
#include <platform/can.hpp>
#include <platform/input_manager.hpp>
#include <platform/interfaces.hpp>
#include <platform/rpi/gpio_manager.hpp>
#include <thread>

namespace dash {

static SPI s_canSpi;
static GPIO s_canGPIO{0, true};
static Clock s_canClock;

void CANManager::initialize() {
    GPIOManager::instance().tick();

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

    MCP2515* driver = static_cast<MCP2515*>(dbc::driveBus.get_driver());
    driver->updateMissCounter();

    std::cout << "Miss counter:" << driver->getMissCounter() << std::endl;
    /*
        std::string error;
        if (driver->probe(error)) {
        std::cout << "Error: " << error << std::endl;
        }
    */
}

}  // namespace dash

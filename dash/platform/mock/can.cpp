#include <can/can_dbc.hpp>
#include <can/mock/can_imgui.hpp>
#include <chrono>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <nfr_can/CAN_interface.hpp>
#include <nfr_can/MCP2515.hpp>
#include <okay/okay.hpp>
#include <platform/can.hpp>
#include <thread>

namespace dash {

void CANManager::initialize() {
    dbc::driveBus.set_driver(std::make_unique<CAN_IMGUI>());

    BaudRate baud500k = BaudRate::kBaud500K;
    if (!dbc::driveBus.init(baud500k)) {
        okay::Engine.logger.error("Failed to initialize CAN bus");

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}

void CANManager::tick() {
    dbc::driveBus.tick_bus();
}

}  // namespace dash

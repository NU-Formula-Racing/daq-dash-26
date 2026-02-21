#include <memory>
#include <okay/core/asset/okay_asset.hpp>
#include <okay/core/level/okay_level_manager.hpp>
#include <okay/core/okay.hpp>
#include <okay/core/renderer/okay_renderer.hpp>
#include <okay/core/renderer/okay_surface.hpp>

#include <nfr_can/CAN_interface.hpp>
#include <nfr_can/MCP2515.hpp>
#include <platform/platform.hpp>
#include <io/neopixel.hpp>

#include <can/can_dbc.hpp>

#include <csignal>
#include <sstream>
#include <string>
#include "io/lights.hpp"
#include "neo-pixel/ws2812-rpi.h"

static void __gameInitialize();
static void __gameUpdate();
static void __gameShutdown();
static void __motorStatusRecv();
static void __exitSignal(int sig);

// clang-format off
static std::vector<ICAN_Message*> g_toPrint = {
    &dbc::bmsStatus::message,
    &dbc::bmsFaults::message,
    &dbc::bmsSoe::message,
    &dbc::ecuDriveStatus::message,
    &dbc::ecuBmsCommandMessage::message,
    &dbc::ecuImplausibility::message,
    &dbc::ecuBrake::message,
    &dbc::ecuThrottle::message,
    &dbc::ecuSetCurrentRearInverter::message,
    &dbc::rearInverterFaultStatus::message,
    &dbc::rearInverterPowerDraw::message,
    &dbc::rearInverterMotorStatus::message,
    &dbc::rearInverterTempStatus::message
};
// clang-format on

int main() {
    okay::SurfaceConfig surfaceConfig;
    okay::Surface surface(surfaceConfig);

    okay::OkayRendererSettings rendererSettings{surfaceConfig};
    auto renderer = okay::OkayRenderer::create(rendererSettings);

    okay::OkayLevelManagerSettings levelManagerSettings;
    auto levelManager = okay::OkayLevelManager::create(levelManagerSettings);

    // attach an interrupt to exit the program on ctrl c
    std::signal(SIGINT, __exitSignal);

    okay::OkayGame::create()
        .addSystems(std::move(levelManager),
                    // std::move(renderer),
                    std::make_unique<okay::OkayAssetManager>())
        .onInitialize(__gameInitialize)
        .onUpdate(__gameUpdate)
        .onShutdown(__gameShutdown)
        .run();

    return 0;
}

static void updateLights() {
    NeopixelDisplay display;
    display.initialize();
    
    uint8_t br = 125;
    Color red = {255, 0, 0, br};
    Color orange = {255, 125, 0, br};
    Color yellow = {255, 255, 0, br};
    Color green = {0, 255, 0, br};
    Color teal = {0, 255, 255, br};
    Color blue = {0, 0, 255, br};
    Color purple = {255, 0, 255, br};
    Color pink = {255, 125, 125, br};

    for (int i = 0; i < 5; i++)
    {
        display.getBar(i).setColor(0,red);
        display.getBar(i).setColor(1,orange);
        display.getBar(i).setColor(2,yellow);
        display.getBar(i).setColor(3,green);
        display.getBar(i).setColor(4,teal);
        display.getBar(i).setColor(5,blue);
        display.getBar(i).setColor(6,purple);
        if (i != 2) {
            display.getBar(i).setColor(7,pink);
        }
    } 
       

}

static void __gameInitialize() {
    std::cout << "Game initialized." << std::endl;
    // Additional game initialization logic
    BaudRate baud500k = BaudRate::kBaud500K;
    if (dbc::driveBus.init(baud500k)) {
        okay::Engine.logger.error("Failed to initialize CAN bus");
    }

    std::ios::sync_with_stdio(false);
    std::cout.tie(nullptr);
    std::cout << "\x1b[?25l"; // hide cursor
    std::cout << "\x1b[?1049h\x1b[2J\x1b[H\x1b[?25l";
    std::cout.flush();

    NeopixelDisplay display;
    VirtualizedNeobar &bar = display.getBar(0);
    bar.setColor(0, { 255, 0, 0, 255});
}

static void __gameShutdown() {
    std::cout << "Game shutdown." << std::endl;
    std::cout << "\x1b[?25h\x1b[?1049l";
    std::cout.flush();
}

static void __gameUpdate() {
    dbc::driveBus.tick_bus();

    std::cout << "\x1b[H\x1b[J";

    std::cout << "NFR26 Development Dashboard\n";

    // Collect all signal strings
    std::vector<std::string> lines;
    for (ICAN_Message* msg : g_toPrint) {
        for (std::uint8_t sigNum = 0; sigNum < msg->get_num_signals(); sigNum++) {
            auto sigId = std::pair{msg->get_id().id, sigNum};

            const char* name = "(unknown)";
            auto it = dbc::meta::signalIdToName.find(sigId);
            if (it != dbc::meta::signalIdToName.end())
                name = it->second;

            lines.emplace_back(std::string{name} + ": " + msg->get_signal(sigNum)->to_string());
        }
    }

    constexpr int COLS = 3;
    constexpr int COL_WIDTH = 32;

    size_t rows = (lines.size() + COLS - 1) / COLS;

    std::ostringstream frame;

    // Print row-wise across columns
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < COLS; c++) {
            size_t idx = r + c * rows;
            if (idx < lines.size()) {
                frame << std::left << std::setw(COL_WIDTH) << lines[idx];
            }
        }
        frame << '\n';
    }

    std::cout << frame.str();
    std::cout.flush();

    updateLights();
}

static void __exitSignal(int sig) {
    okay::Engine.logger.info("Exit signal received: {}", sig);
    //okay::Engine.shutdown();
}

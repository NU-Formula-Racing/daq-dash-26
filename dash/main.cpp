#include <memory>
#include <okay/core/asset/okay_asset.hpp>
#include <okay/core/level/okay_level_manager.hpp>
#include <okay/core/okay.hpp>
#include <okay/core/renderer/okay_renderer.hpp>
#include <okay/core/renderer/okay_surface.hpp>

#include <nfr_can/CAN_interface.hpp>
#include <nfr_can/MCP2515.hpp>
#include <platform/platform.hpp>

#include <can/can_dbc.hpp>

#include <csignal>
#include <sstream>
#include <string>
#include <nfr_can/virtual_timer.hpp>
#include <io/lights.hpp>

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

// heartbeat message
inline uint64_t g_heartbeatCount = 0;
inline VirtualTimerGroup g_timerGroup;

TX_can_msg_config g_heartbeat_conf = {
    .bus = dbc::driveBus,
    .id = 0x510,
    .extended = false,
    .length = 8,
    .period = 1000,
    .timerGroup = g_timerGroup
}; 

inline CAN_Signal_UINT64 g_heartbeatSignal = MakeSignalExp(uint64_t, 0, 64, 1.0, 0.0);
inline TX_CAN_Message(1) g_heartbeatMessage{g_heartbeat_conf, g_heartbeatSignal};

inline dash::platform::SPI g_canSpi;
inline dash::platform::GPIO g_canGPIO{"gpiochip0", 0, true};
inline dash::platform::Clock g_canClock;

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
    dash::NeopixelDisplay display;
    display.initialize();
    
    uint8_t br = 125;
    glm::vec4 red = {255, 0, 0, br};
    glm::vec4 orange = {255, 125, 0, br};
    glm::vec4 yellow = {255, 255, 0, br};
    glm::vec4 green = {0, 255, 0, br};
    glm::vec4 teal = {0, 255, 255, br};
    glm::vec4 blue = {0, 0, 255, br};
    glm::vec4 purple = {255, 0, 255, br};
    glm::vec4 pink = {255, 125, 125, br};

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
    g_timerGroup.AddTimer(1000, []() { g_heartbeatCount++; });
    dbc::driveBus.set_driver(std::make_unique<MCP2515>(g_canSpi, g_canGPIO, g_canClock));

    // check for errors
    if (g_canGPIO.checkError()) {
        okay::Engine.logger.error("Failed to initialize GPIO");
    }

    // Additional game initialization logic
    BaudRate baud500k = BaudRate::kBaud500K;
    if (!dbc::driveBus.init(baud500k)) {
        okay::Engine.logger.error("Failed to initialize CAN bus");

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    // std::ios::sync_with_stdio(false);
    // std::cout.tie(nullptr);
    // std::cout << "\x1b[?25l"; // hide cursor
    // std::cout << "\x1b[?1049h\x1b[2J\x1b[H\x1b[?25l";
    // std::cout.flush();
}

static void __gameShutdown() {
    // std::cout << "Game shutdown." << std::endl;
    // std::cout << "\x1b[?25h\x1b[?1049l";
    // std::cout.flush();
}

static void __gameUpdate() {
    g_timerGroup.Tick(g_canClock.monotonicMs());
    dbc::driveBus.tick_bus();

    // std::cout << "\x1b[H\x1b[J";

    // std::cout << "NFR26 Development Dashboard\n";

    // // Collect all signal strings
    // std::vector<std::string> lines;
    // for (ICAN_Message* msg : g_toPrint) {
    //     for (std::uint8_t sigNum = 0; sigNum < msg->get_num_signals(); sigNum++) {
    //         auto sigId = std::pair{msg->get_id().id, sigNum};

    //         const char* name = "(unknown)";
    //         auto it = dbc::meta::signalIdToName.find(sigId);
    //         if (it != dbc::meta::signalIdToName.end())
    //             name = it->second;

    //         lines.emplace_back(std::string{name} + ": " + msg->get_signal(sigNum)->to_string());
    //     }
    // }

    // constexpr int COLS = 3;
    // constexpr int COL_WIDTH = 32;

    // size_t rows = (lines.size() + COLS - 1) / COLS;

    // std::ostringstream frame;

    // // Print row-wise across columns
    // for (size_t r = 0; r < rows; r++) {
    //     for (size_t c = 0; c < COLS; c++) {
    //         size_t idx = r + c * rows;
    //         if (idx < lines.size()) {
    //             frame << std::left << std::setw(COL_WIDTH) << lines[idx];
    //         }
    //     }
    //     frame << '\n';
    // }

    // std::cout << frame.str();
    // std::cout.flush();

    updateLights();
}

static void __exitSignal(int sig) {
    okay::Engine.logger.info("Exit signal received: {}", sig);
    //okay::Engine.shutdown();
}

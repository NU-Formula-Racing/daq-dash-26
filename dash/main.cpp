#include <memory>
#include <okay/core/asset/okay_asset.hpp>
#include <okay/core/level/okay_level_manager.hpp>
#include <okay/core/okay.hpp>
#include <okay/core/renderer/okay_renderer.hpp>
#include <okay/core/renderer/okay_surface.hpp>

#include <nfr_can/CAN_interface.hpp>
#include <nfr_can/MCP2515.hpp>
#include <nfr_can/virtual_timer.hpp>
#include <platform/platform.hpp>
#include <io/lights.hpp>
#include <can/can_dbc.hpp>

#include <csignal>
#include <string>
#include <sstream>
#include <iomanip>

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

TX_can_msg_config g_heartbeat_conf = {.bus = dbc::driveBus,
                                      .id = 0x510,
                                      .extended = false,
                                      .length = 8,
                                      .period = 1000,
                                      .timerGroup = g_timerGroup};

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
                    std::make_unique<dash::NeopixelDisplay>(),
                    std::make_unique<okay::OkayAssetManager>())
        .onInitialize(__gameInitialize)
        .onUpdate(__gameUpdate)
        .onShutdown(__gameShutdown)
        .run();

    return 0;
}

static void __updateLights() {
    dash::NeopixelDisplay* display = okay::Engine.systems.getSystemChecked<dash::NeopixelDisplay>();

    glm::vec4 red = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 blue = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    // get time since the start of the program in ms
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    std::chrono::duration<float, std::milli> ms = now.time_since_epoch();
    float brightness = (std::sin(ms.count() / 1000.0f) + 1.0f) / 2.0f;

    for (int i = 0; i < 5; i++) {
        // if (i == 2) continue;
        for (int j = 0; j < display->getBar(i).numPixels(); j++) {
            float t = (float)j / (float)display->getBar(i).numPixels();
            glm::vec4 color = red;
            // mix between red green and blue with a gradient
            if (t < 0.5f) {
                // Map t from [0, 0.5] to [0, 1] for R->G
                color = glm::mix(red, green, t * 2.0f);
            } else {
                // Map t from [0.5, 1] to [0, 1] for G->B
                color = glm::mix(green, blue, (t - 0.5f) * 2.0f);
            }

            // color.a = brightness;
            display->getBar(i).setColor(j, color);
        }
    }

    display->updateDisplay();
}

static void __flushScreen() {
    // Collect all signal strings
    std::vector<std::string> lines;
    lines.reserve(256);

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
    const size_t rows = (lines.size() + COLS - 1) / COLS;

    // Build one complete frame
    std::string out;
    out.reserve(8192);

    // Home cursor
    out += "\x1b[H";
    out += "NFR26 Development Dashboard\n\n";

    // Format into columns
    std::ostringstream grid;
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < COLS; c++) {
            const size_t idx = r + c * rows;
            if (idx < lines.size()) {
                grid << std::left << std::setw(COL_WIDTH) << lines[idx];
            } else {
                grid << std::setw(COL_WIDTH) << "";
            }
        }
        grid << '\n';
    }

    out += grid.str();

    // Now clear the rest of the screen
    out += "\x1b[J";

    // print to stdout, and flush
    std::cout << out;
    std::cout.flush();
}

static void __gameInitialize() {
    std::cout << "Game initialized." << std::endl;
    g_timerGroup.AddTimer(1000, []() { g_heartbeatCount++; });
    g_timerGroup.AddTimer(20, []() { __flushScreen(); });
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

    std::ios::sync_with_stdio(false);
    std::cout.tie(nullptr);
    std::cout << "\x1b[?25l";  // hide cursor
    std::cout << "\x1b[?1049h\x1b[2J\x1b[H\x1b[?25l";
    std::cout.flush();
}

static void __gameShutdown() {
    std::cout << "Game shutdown." << std::endl;
    std::cout << "\x1b[?25h\x1b[?1049l";
    std::cout.flush();
    okay::Engine.shutdown();
}

static void __gameUpdate() {
    g_timerGroup.Tick(g_canClock.monotonicMs());
    dbc::driveBus.tick_bus();
    __updateLights();
}

static void __exitSignal(int sig) {
    okay::Engine.logger.info("Exit signal received: {}", sig);
    okay::Engine.shutdown();
}

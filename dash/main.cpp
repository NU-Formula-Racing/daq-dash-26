#include <can/can_dbc.hpp>
#include <csignal>
#include <math.h>
#include <nfr_can/CAN_interface.hpp>
#include <nfr_can/virtual_timer.hpp>
#include <okay/okay.hpp>
#include <platform/can.hpp>
#include <platform/interfaces.hpp>
#include <platform/neopixel_manager.hpp>
#include <sstream>
#include <string>

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
    // &dbc::ecuDriveStatus::message,
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

    okay::RendererSettings rendererSettings{
        .surfaceConfig = surfaceConfig,
        .pipeline = okay::RenderPipeline::create(std::make_unique<okay::ScenePass>())};

    // attach an interrupt to exit the program on ctrl c
    std::signal(SIGINT, __exitSignal);

    okay::Game::create()
        .addSystems(std::make_unique<okay::Renderer>(std::move(rendererSettings)),
                    std::make_unique<dash::NeopixelManager>(),
                    std::make_unique<okay::AssetManager>(),
                    std::make_unique<okay::TweenEngine>(),
                    std::make_unique<dash::CANManager>())
        .onInitialize(__gameInitialize)
        .onUpdate(__gameUpdate)
        .onShutdown(__gameShutdown)
        .run();

    return 0;
}

static void __gameInitialize() {
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
}

static void __exitSignal(int sig) {
    okay::Engine.logger.info("Exit signal received: {}", sig);
    okay::Engine.shutdown();
}

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

static std::size_t __frameCount = 0;

// clang-format off
static std::vector<ICAN_Message*> g_toPrint = {
    &dbc::ecuBrake::message,
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
        .addSystems(
            // std::make_unique<okay::Renderer>(std::move(rendererSettings)),
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
    // std::ios::sync_with_stdio(false);
    // std::cout.tie(nullptr);
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
    std::cout << "\x1b[H";
    std::cout << "NFR26 Development Dashboard\n";

    // Collect all signal strings
    for (ICAN_Message* msg : g_toPrint) {
        for (std::uint8_t sigNum = 0; sigNum < msg->get_num_signals(); sigNum++) {
            auto sigId = std::pair{msg->get_id().id, sigNum};

            const char* name = "(unknown)";
            auto it = dbc::meta::signalIdToName.find(sigId);
            if (it != dbc::meta::signalIdToName.end())
                name = it->second;

            std::cout << std::string{name} << ": " << msg->get_signal(sigNum)->to_string() << '\n';
        }
    }

    std::cout << "Frame count: " << __frameCount++ << '\n';
    std::cout << "Delta time: " << okay::Engine.time->deltaTimeMs() << '\n';

    std::cout << "\x1b[J";
    std::cout.flush();
}

static void __exitSignal(int sig) {
    okay::Engine.logger.info("Exit signal received: {}", sig);
    okay::Engine.shutdown();
}

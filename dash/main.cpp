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
#include <string>

static void __gameInitialize();
static void __gameUpdate();
static void __gameShutdown();
static void __motorStatusRecv();
static void __exitSignal(int sig);


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

static void __gameInitialize() {
    std::cout << "Game initialized." << std::endl;
    // Additional game initialization logic

    BaudRate baud500k = BaudRate::kBaud500K;
    if (dbc::driveBus.init(baud500k)) {
        okay::Engine.logger.error("Failed to initialize CAN bus");
        while (true) {
        }
    }
}

static void __gameShutdown() {
    std::cout << "Game shutdown." << std::endl;
    // Cleanup logic before game shutdown
}

static void __gameUpdate() {
    can::timerGroup.Tick(can::canClock.monotonicMs());
    can::bus.tick_bus();
}

static void __motorStatusRecv() {
    std::cout << "RPM: " << can::RPM->get() << std::endl;
    std::cout << "Motor Current: " << can::Motor_Current->get() << std::endl;
    std::cout << "DC Voltage: " << can::DC_Voltage->get() << std::endl;
    std::cout << "DC Current: " << can::DC_Current->get() << std::endl;
}

static void __exitSignal(int sig) {
    okay::Engine.logger.info("Exit signal received: {}", sig);
    okay::Engine.shutdown();
}
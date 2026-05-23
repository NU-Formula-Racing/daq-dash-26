#include "ui/drive_page.hpp"
#include "ui/page.hpp"

#include <okay/okay.hpp>

#include <can/can_dbc.hpp>
#include <csignal>
#include <math.h>
#include <memory>
#include <platform/can.hpp>
#include <platform/interfaces.hpp>
#include <platform/neopixel_manager.hpp>
#include <sstream>

static void __exitSignal(int sig);

int main() {
    okay::SurfaceConfig surfaceConfig;
    surfaceConfig.width = 800;
    surfaceConfig.height = 480;
    okay::Surface surface(surfaceConfig);

    okay::RendererSettings rendererSettings{
        .surfaceConfig = surfaceConfig,
        .pipeline = okay::RenderPipeline::create(std::make_unique<okay::ScenePass>()),
        .enableIMGUI = true,
    };

    std::unique_ptr<dash::PageManager> pageManager = std::make_unique<dash::PageManager>(
        dash::PageEntry::create(std::make_unique<dash::DrivePage>())
            .activeWhen([]() {
                return true;
            })
            .withPriority(0));

    // attach an interrupt to exit the program on ctrl c
    std::signal(SIGINT, __exitSignal);

    auto game = okay::Game::create().addSystems(
        std::make_unique<okay::Renderer>(std::move(rendererSettings)),
        std::make_unique<dash::NeopixelManager>(),
        std::make_unique<okay::AssetManager>(),
        std::make_unique<okay::TweenEngine>(),
        std::make_unique<dash::CANManager>(),
        std::make_unique<okay::ECS>(),
        std::move(pageManager));

    okay::registerBuiltinComponentsAndSystems();
    game.run();

    return 0;
}

static void __exitSignal(int sig) {
    okay::Engine.logger.info("Exit signal received: {}", sig);
    okay::Engine.shutdown();
}

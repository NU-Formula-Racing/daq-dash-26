#include "ui/brokers_debug_page.hpp"
#include "ui/car_state.hpp"
#include "ui/components/rotate.hpp"
#include "ui/debug_page.hpp"
#include "ui/drive_page.hpp"
#include "ui/imu_pdm_tlm_debug_page.hpp"
#include "ui/inputs.hpp"
#include "ui/error_page.hpp"
#include "ui/inverter_page.hpp"
#include "ui/page.hpp"
#include "ui/inputs.hpp"

#include <okay/okay.hpp>

#include <can/can_dbc.hpp>
#include <csignal>
#include <math.h>
#include <memory>
#include <platform/can.hpp>
#include <platform/interfaces.hpp>
#include <platform/neopixel_manager.hpp>

static void __exitSignal(int sig);

static okay::ECSEntity s_performanceUIEntity;

using namespace dash;

int main() {
    okay::SurfaceConfig surfaceConfig;
    surfaceConfig.width = 800;
    surfaceConfig.height = 480;
    surfaceConfig.title = "NFR26 Dashboard";

    okay::RendererSettings rendererSettings{
        .surfaceConfig = surfaceConfig,
        .pipeline = okay::RenderPipeline::create(std::make_unique<okay::ScenePass>()),
        .enableIMGUI = true,
    };

    std::unique_ptr<PageManager> pageManager = std::make_unique<PageManager>(
        // Drive Page
        PageEntry::create(std::make_unique<DrivePage>()).withPriority(0).withPageNumber(0),
        // Debug/Error Page
        PageEntry::create(std::make_unique<DebugPage>())
            .forceOverrideWhen([]() {
                return CarState::hardFaultPresent();
            })
            .withPriority(1)
            .withPageNumber(1),
        // Brokers Page
        PageEntry::create(std::make_unique<BrokersDebugPage>()).withPriority(0).withPageNumber(2),
        // Misc LV Page
        PageEntry::create(std::make_unique<IMUPDMTLMDebugPage>())
            .withPriority(0)
            .withPageNumber(3),
        PageEntry::create(std::make_unique<InverterPage>()).withPriority(0).withPageNumber(4)
    );

    // attach an interrupt to exit the program on ctrl c
    std::signal(SIGINT, __exitSignal);

    auto game = okay::Game::create().addSystems(
        std::make_unique<okay::Renderer>(std::move(rendererSettings)),
        std::make_unique<NeopixelManager>(),
        std::make_unique<okay::AssetManager>(),
        std::make_unique<okay::TweenEngine>(),
        std::make_unique<CANManager>(),
        std::make_unique<okay::ECS>(),
        std::move(pageManager));

    // must happen after first init() because of IMGUI
    // IMGUI overrides glfwcallbacks in the native build
    // this is a hack until okay engine get it's own
    // input system
    game.onInitialize([]() {
        input::leftButton.onDown([]() {
            okay::Engine.systems.getSystemChecked<PageManager>()->switchPageLeft();
        });

        input::rightButton.onDown([]() {
            okay::Engine.systems.getSystemChecked<PageManager>()->switchPageRight();
        });
    });

    okay::registerBuiltinComponentsAndSystems();
    okay::ecs::registerComponent<RotateComponent>();
    okay::ecs::registerSystem(std::make_unique<RotateSystem>());
    dbc::bmsStatus::imdState->set(1);

    game.run();
    return 0;
}

static void __exitSignal(int sig) {
    okay::Engine.logger.info("Exit signal received: {}", sig);
    okay::Engine.shutdown();
}

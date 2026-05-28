#include "okay/core/ecs/ecs.hpp"
#include "ui/car_state.hpp"
#include "ui/components/rotate.hpp"
#include "ui/debug_page.hpp"
#include "ui/drive_page.hpp"
#include "ui/error_page.hpp"
#include "ui/inverter_page.hpp"
#include "ui/page.hpp"
#include "ui/shared_elements.hpp"

#include <okay/okay.hpp>

#include <can/can_dbc.hpp>
#include <csignal>
#include <math.h>
#include <memory>
#include <platform/button.hpp>
#include <platform/can.hpp>
#include <platform/interfaces.hpp>
#include <platform/neopixel_manager.hpp>
#include <sstream>

static void __exitSignal(int sig);

static bool s_debugPageActive{false};
static okay::ECSEntity s_performanceUIEntity;

static dash::Button downButton{20};
inline dash::Button leftButton{16};
inline dash::Button rightButton{12};

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

    downButton.onDown([]() {
        s_debugPageActive = !s_debugPageActive;
    });

    leftButton.onDown([]() {
        if (s_performanceUIEntity.isValid()) {
            s_performanceUIEntity.destroy();
        } else {
            s_performanceUIEntity =
                okay::ecs::uiEntity(LAMBDA_WRAP(dash::SharedElements::get().buildPerformanceUI), 4);
        }
    });

    std::unique_ptr<dash::PageManager> pageManager = std::make_unique<dash::PageManager>(
        dash::PageEntry::create(std::make_unique<dash::DrivePage>())
            .activeWhen([]() {
                return !s_debugPageActive;
            })
            .withPriority(0),
        dash::PageEntry::create(std::make_unique<dash::ErrorPage>())
            .activeWhen([]() {
                return false;
            })
            .withPriority(0),
        dash::PageEntry::create(std::make_unique<dash::DebugPage>())
            .activeWhen([]() {
                return s_debugPageActive || dash::CarState::hardFaultPresent();
            })
            .withPriority(1),
        dash::PageEntry::create(std::make_unique<dash::InverterPage>())
            .activeWhen([]() {
                return true;
            })
            .withPriority(100));

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
    okay::ecs::registerComponent<dash::RotateComponent>();
    okay::ecs::registerSystem(std::make_unique<dash::RotateSystem>());

    dbc::bmsStatus::imdState->set(1);

    game.run();

    return 0;
}

static void __exitSignal(int sig) {
    okay::Engine.logger.info("Exit signal received: {}", sig);
    okay::Engine.shutdown();
}

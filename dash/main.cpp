#include <can/can_dbc.hpp>
#include <csignal>
#include <iostream>
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

static void handleInverterFaults(std::stringstream& frame);
static void handleBMSFaults(std::stringstream& frame);
static void handleECUFaults(std::stringstream& frame);

static std::size_t __frameCount = 0;

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

static void __exitSignal(int sig) {
    okay::Engine.logger.info("Exit signal received: {}", sig);
    okay::Engine.shutdown();
}

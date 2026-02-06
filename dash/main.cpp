#include <okay/core/asset/okay_asset.hpp>
#include <okay/core/level/okay_level_manager.hpp>
#include <okay/core/okay.hpp>
#include <okay/core/renderer/okay_renderer.hpp>
#include <okay/core/renderer/okay_surface.hpp>

#include <nfr_can/CAN_interface.hpp>
#include <nfr_can/MCP2515.hpp>
#include <platform/platform.hpp>

#include <nfr_can/virtual_timer.hpp>

static void __gameInitialize();
static void __gameUpdate();
static void __gameShutdown();
static void __motorStatusRecv();


namespace can {

dash::platform::SPI canSpi;
dash::platform::GPIO canGPIO{"gpiochip0", 0, true};
dash::platform::Clock canClock;
VirtualTimerGroup timerGroup;
MCP2515 canDriver{canSpi, canGPIO, canClock};
CAN_Bus bus{canDriver};

CAN_Signal_INT16 RPM = MakeSignalExp(int16_t, 0, 16, 1, 0);
CAN_Signal_INT16 Motor_Current = MakeSignalExp(int16_t, 16, 16, 0.1, 0);
CAN_Signal_INT16 DC_Voltage = MakeSignalExp(int16_t, 32, 16, 0.1, 0);
CAN_Signal_INT16 DC_Current = MakeSignalExp(int16_t, 48, 16, 0.1, 0);

CAN_Signal_INT16 APPS1_Throttle = MakeSignalExp(int16_t, 0, 16, 1, 0);
CAN_Signal_INT16 APPS2_Throttle = MakeSignalExp(int16_t, 16, 16, 1, 0);

RX_CAN_Message(4) motor_status(bus, 0x281, false, 8, __motorStatusRecv, RPM,
                               Motor_Current, DC_Voltage, DC_Current);
TX_CAN_Message(2) ECU_Throttle(bus, 0x202, false, 4, 1000, timerGroup,
                               APPS1_Throttle, APPS2_Throttle);

}; // namespace can

int main() {
  okay::SurfaceConfig surfaceConfig;
  okay::Surface surface(surfaceConfig);

  okay::OkayRendererSettings rendererSettings{surfaceConfig};
  auto renderer = okay::OkayRenderer::create(rendererSettings);

  okay::OkayLevelManagerSettings levelManagerSettings;
  auto levelManager = okay::OkayLevelManager::create(levelManagerSettings);

  okay::OkayGame::create()
      .addSystems(
            std::move(levelManager), 
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
  bool canInit = can::canDriver.begin(baud500k);
  if (!canInit) {
    okay::Engine.logger.error("Failed to initialize CAN bus");
    while (true) {}
  }
}

static void __gameUpdate() {
//   okay::Engine.logger.info("Game update.");
  can::APPS1_Throttle->set(33);
  can::APPS2_Throttle->set(55);
  bool sent = can::bus.send(can::ECU_Throttle);
  if (!sent) {
    okay::Engine.logger.error("Failed to send message");
    while (true) {}
  }


  can::timerGroup.Tick(can::canClock.monotonicMs());
  can::bus.tick_bus();
  can::canDriver.updateMissCounter();
}

static void __gameShutdown() {
  std::cout << "Game shutdown." << std::endl;
  // Cleanup logic before game shutdown
}

static void __motorStatusRecv() {
  std::cout << "RPM: " << can::RPM->get() << std::endl;
  std::cout << "Motor Current: " << can::Motor_Current->get() << std::endl;
  std::cout << "DC Voltage: " << can::DC_Voltage->get() << std::endl;
  std::cout << "DC Current: " << can::DC_Current->get() << std::endl;
}
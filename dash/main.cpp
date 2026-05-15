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

// clang-format off
static std::vector<ICAN_Message*> g_toPrint = {
    &dbc::bmsStatus::message,
    &dbc::pdmBatVolt::message,
    &dbc::ecuBmsCommandMessage::message,
    &dbc::ecuImplausibility::message,
    &dbc::ecuBrake::message,
    &dbc::ecuThrottle::message,
    &dbc::ecuSetCurrentRearInverter::message,
    &dbc::rearInverterFaultStatus::message,
    &dbc::rearInverterPowerDraw::message,
    &dbc::rearInverterMotorStatus::message,
    &dbc::rearInverterTempStatus::message,
    // &dbc::brBrokerSus::message,
    // &dbc::brBrokerCanErrorMsg::message,
    // &dbc::brBrokerTemp1::message,
    // &dbc::brBrokerTemp2::message
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

    std::stringstream frame;
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

    handleInverterFaults(frame);
    handleECUFaults(frame);
    handleBMSFaults(frame);

    std::cout << frame.str();
    std::cout << "\x1b[J";
    std::cout << "\x1b[H";
    std::cout.flush();
}

static void __exitSignal(int sig) {
    okay::Engine.logger.info("Exit signal received: {}", sig);
    okay::Engine.shutdown();
}

void handleInverterFaults(std::stringstream& frame) {
    std::uint8_t rearInverterError = dbc::rearInverterFaultStatus::faultCode->get();

    std::unordered_map<int, std::string> faultCodes = {{0x00, "NONE"},
                                                       {0x01, "OVER_VOLTAGE"},
                                                       {0x02, "UNDER_VOLTAGE"},
                                                       {0x03, "DRV_FAULT"},
                                                       {0x04, "ABS_OVER_CURRENT"},
                                                       {0x05, "OVER_TEMP_FET"},
                                                       {0x06, "OVER_TEMP_MOTOR"},
                                                       {0x07, "GATE_DRIVER_OVER_VOLTAGE"},
                                                       {0x08, "GATE_DRIVER_UNDER_VOLTAGE"},
                                                       {0x09, "MCU_UNDER_VOLTAGE"},
                                                       {0x0A, "BOOTING_FROM_WATCHDOG_RESET"},
                                                       {0x0B, "ENCODER_SPI_FAULT"},
                                                       {0x0C, "ENCODER_SINCOS_BELOW_MIN_AMPLITUDE"},
                                                       {0x0D, "ENCODER_SINCOS_ABOVE_MAX_AMPLITUDE"},
                                                       {0x0E, "FLASH_CORRUPTION"},
                                                       {0x0F, "HIGH_OFFSET_CURRENT_SENSOR_1"},
                                                       {0x10, "HIGH_OFFSET_CURRENT_SENSOR_2"},
                                                       {0x11, "HIGH_OFFSET_CURRENT_SENSOR_3"},
                                                       {0x12, "UNBALANCED_CURRENTS"},
                                                       {0x13, "BRK_FAULT"},
                                                       {0x14, "RESOLVER_LOT"},
                                                       {0x15, "RESOLVER_DOS"},
                                                       {0x16, "RESOLVER_LOS"},
                                                       {0x17, "FLASH_CORRUPTION_APP_CFG"},
                                                       {0x18, "FLASH_CORRUPTION_MC_CFG"},
                                                       {0x19, "ENCODER_NO_MAGNET"},
                                                       {0x1A, "ENCODER_MAGNET_TOO_STRONG"},
                                                       {0x1B, "PHASE_FILTER_FAULT"}};

    if (rearInverterError > 0) {
        std::string errorStr = "UNKNOWN";
        if (faultCodes.find(rearInverterError) != faultCodes.end()) {
            errorStr = faultCodes[rearInverterError];
        }
        frame << "Rear Inverter Fault: " << errorStr << " (0x" << std::hex << (int)rearInverterError
              << std::dec << ")\n";
    }
}

void handleECUFaults(std::stringstream& frame) {
    bool implPres = dbc::ecuImplausibility::implausibilityPresent->get();
    bool appsImpl = dbc::ecuImplausibility::appssDisagreementImp->get();
    bool bppcImpl = dbc::ecuImplausibility::appssDisagreementImp->get();
    bool brakeInv = dbc::ecuImplausibility::brakeInvalidImp->get();
    bool appsInv = dbc::ecuImplausibility::appssInvalidImp->get();

    if (implPres) {  // implausibility present
        frame << "ECU Implausibility Error: Implausibility Present\n";
    }
    if (appsImpl) {  // apps implausibility
        frame << "ECU Implausibility Error: APPSs Disagreement Implausibility\n";
    }
    if (bppcImpl) {  // bppc implausibility
        frame << "ECU Implausibility Error: BPPC Implausibility\n";
    }
    if (brakeInv) {  // brake invalid implausibility
        frame << "ECU Implausibility Error: Brake Invalid Implausibility\n";
    }
    if (appsInv) {  // apps invalid implausibility
        frame << "ECU Implausibility Error: Apps Invalid Implausibility\n";
    }
}

// handle BMS faults
void handleBMSFaults(std::stringstream& frame) {
    // not implemented yet!
}
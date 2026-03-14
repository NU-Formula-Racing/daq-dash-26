#include <memory>
#include <okay/core/okay.hpp>
#include <okay/core/renderer/okay_renderer.hpp>
#include <okay/core/renderer/okay_surface.hpp>
#include <okay/core/level/okay_level_manager.hpp>
#include <okay/core/asset/okay_asset.hpp>
#include <okay/core/logging/okay_logger.hpp>
#include <okay/core/renderer/imgui/okay_imgui.hpp>
#include <okay/core/tween/okay_tween.hpp>
#include <okay/core/renderer/passes/scene_pass.hpp>

#include <nfr_can/CAN_interface.hpp>
#include <nfr_can/virtual_timer.hpp>
#include "platform/platform.hpp"
#include <io/lights.hpp>

#include "can/can_dbc.hpp"

#include <csignal>
#include <string>
#include <sstream>
#include <math.h>
#include "glm/ext/vector_float4.hpp"

static void __gameInitialize();
static void __gameUpdate();
static void __gameShutdown();
static void __motorStatusRecv();
static void __exitSignal(int sig);

bool re_pressed = false;
void re_button_callback(){
    re_pressed = true;
}
void re_up_cb(){
    re_pressed = false;
}

int16_t encoder_counter = 0;
void re_right_cb(){
    encoder_counter++;;
}
void re_left_cb(){
    encoder_counter--;
}

bool down_pressed = false;
void down_down_cb(){
    down_pressed = true;
}
void down_up_cb(){
    down_pressed = false;
}

bool right_pressed = false;
void right_down_cb(){
    right_pressed = true;
}
void right_up_cb(){
    right_pressed = false;
}

bool left_pressed = false;
void left_down_cb(){
    left_pressed = true;
}
void left_up_cb(){
    left_pressed = false;
}

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

// heartbeat message
inline uint64_t g_heartbeatCount = 0;
inline VirtualTimerGroup g_timerGroup;
inline dash::platform::Clock g_canClock;

TX_can_msg_config g_heartbeat_conf = {.bus = dbc::driveBus,
                                      .id = 0x510,
                                      .extended = false,
                                      .length = 8,
                                      .period = 1000,
                                      .timerGroup = g_timerGroup};

inline CAN_Signal_UINT64 g_heartbeatSignal = MakeSignalExp(uint64_t, 0, 64, 1.0, 0.0);
inline TX_CAN_Message(1) g_heartbeatMessage{g_heartbeat_conf, g_heartbeatSignal};


int main() {
    okay::SurfaceConfig surfaceConfig;
    okay::Surface surface(surfaceConfig);

    okay::OkayLevelManagerSettings levelManagerSettings;
    auto levelManager = okay::OkayLevelManager::create(levelManagerSettings);

    okay::OkayRendererSettings rendererSettings{
        .surfaceConfig = surfaceConfig,
        .pipeline = okay::OkayRenderPipeline::create(
            std::make_unique<okay::ScenePass>()
        )
    };
     

    // attach an interrupt to exit the program on ctrl c
    std::signal(SIGINT, __exitSignal);

    okay::OkayGame::create()
        .addSystems(std::move(levelManager),
                    std::make_unique<okay::OkayRenderer>(std::move(rendererSettings)),
                    std::make_unique<dash::NeopixelManager>(),
                    std::make_unique<okay::OkayAssetManager>(),
                    std::make_unique<okay::OkayTweenEngine>(),
                    std::make_unique<okay::OkayIMGUI>())
        .onInitialize(__gameInitialize)
        .onUpdate(__gameUpdate)
        .onShutdown(__gameShutdown)
        .run();

    return 0;
}

static void __updateLights() {
    dash::NeopixelManager* display = okay::Engine.systems.getSystemChecked<dash::NeopixelManager>();

    glm::vec4 red = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 blue = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 purple = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

    // get time since the start of the program in ms
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    std::chrono::duration<float, std::milli> ms = now.time_since_epoch();
    float brightness = (std::sin(ms.count() / 1000.0f) + 1.0f) / 2.0f;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < display->getBar(i).numPixels(); j++) {
            float t = (float)j / (float)display->getBar(i).numPixels();
            glm::vec4 a = red;
            glm::vec4 b = green;
            glm::vec4 c = blue;

            if (i == 2) {
                a = purple;
                b = black;
                c = purple;
            }

            glm::vec4 color = a;
            // mix between red green and blue with a gradient
            if (t < 0.5f) {
                // Map t from [0, 0.5] to [0, 1] for R->G
                color = glm::mix(a, b, t * 2.0f);
            } else {
                // Map t from [0.5, 1] to [0, 1] for G->B
                color = glm::mix(b, c, (t - 0.5f) * 2.0f);
            }

            color.a = brightness;
            display->getBar(i).setColor(j, color);
        }
    }

    display->updateDisplay();
}

static void __interruptInitialize(){
    reButton.onDown(re_button_callback);
    reButton.onUp(re_up_cb);
    rotaryEncoder.onLeft(re_left_cb);
    rotaryEncoder.onRight(re_right_cb);

    downButton.onDown(down_down_cb);
    downButton.onUp(down_up_cb);

    rightButton.onDown(right_down_cb);
    rightButton.onUp(right_up_cb);

    leftButton.onDown(left_down_cb);
    leftButton.onUp(left_up_cb);
}

static void __gameInitialize() {
    std::cout << "Game initialized." << std::endl;
    g_timerGroup.AddTimer(1000, []() { g_heartbeatCount++; });
    g_timerGroup.AddTimer(20, []() { __flushScreen(); });
    
    dash::platform::configureCANDriver(dbc::driveBus);

    // Additional game initialization logic
    BaudRate baud500k = BaudRate::kBaud500K;
    if (!dbc::driveBus.init(baud500k)) {
        okay::Engine.logger.error("Failed to initialize CAN bus");

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    __interruptInitialize();

    std::ios::sync_with_stdio(false);
    std::cout.tie(nullptr);
    std::cout << "\x1b[?25l"; // hide cursor
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
    dash::platform::preUpdate();

    g_timerGroup.Tick(g_canClock.monotonicMs());
    dbc::driveBus.tick_bus();
    __updateLights();

    dash::platform::tick();

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

    std::string out = "\nINPUT DEMO:\n";
    out += "Down Button: " + std::string(downButton.isDown() ? "held" : "not held") + "\n";
    out += "Right Button: " + std::string(rightButton.isDown() ? "held" : "not held") + "\n";
    out += "Left Button: " + std::string(leftButton.isDown() ? "held" : "not held") + "\n";
    out += "RE Button: " + std::string(reButton.isDown() ? "held" : "not held") + "\n";
    out += "RE Count Val: " + std::to_string(encoder_counter);

    // print to stdout, and flush
    std::cout << out;
    std::cout.flush();
    std::cout.flush();
    dash::platform::postUpdate();
}

static void __exitSignal(int sig) {
    okay::Engine.logger.info("Exit signal received: {}", sig);
    okay::Engine.shutdown();
}

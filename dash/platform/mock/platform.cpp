#include <platform/platform.hpp>
#include <can/mock/can_imgui.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace dash {

void Platform::tick() {
    
}

void Platform::configureCANDriver(CAN_Bus& bus) {
    auto canImgui = std::make_unique<CAN_IMGUI>();
    bus.set_driver(std::move(canImgui));
}

}  // namespace dash

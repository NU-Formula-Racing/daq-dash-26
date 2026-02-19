#include "can_imgui.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

bool CAN_IMGUI::init(const BaudRate baud) {
    return true;
} // do nothing

bool CAN_IMGUI::send(const CAN_Frame& msg) {
    return true;
} // do nothing

bool CAN_IMGUI::recv(CAN_Frame& msg) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    ImGui::Begin("CAN");
    if (ImGui::CollapsingHeader("PDMInfo", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputFloat("batVoltage", &pdmInfo.batVoltage);
        ImGui::InputFloat("batCurr", &pdmInfo.batCur);
    }
    if (ImGui::CollapsingHeader("bms_temp0", ImGuiTreeNodeFlags_DefaultOpen)) {
        // ImGui::InputFloat("cell0", &);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    
    
    return false;
}

uint32_t CAN_IMGUI::time_ms() {
    return 0;
} // do nothing


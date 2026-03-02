#include "can_imgui.hpp"
#include <string.h>
#include "can/can_dbc.hpp"
#include "imgui.h"
#include "nfr_can/CAN_interface.hpp"
#include <cstdint>
#include <cstdlib>

bool CAN_IMGUI::init(const BaudRate baud) {
    return true;
} // do nothing

bool CAN_IMGUI::send(const CAN_Frame& msg) {
    return true;
} // do nothing

bool CAN_IMGUI::recv(CAN_Frame& msg) {
    drawUI();
    return false;
}

void CAN_IMGUI::drawUI() {
    ImGui::Begin("CAN");

    for (ICAN_Message* message : dbc::driveBus.get_messages()) {
        uint32_t messageId { message->get_id().id };

        if (ImGui::CollapsingHeader(dbc::meta::messageIdToName.at(messageId), ImGuiTreeNodeFlags_DefaultOpen)) {
            for (uint8_t sigNum {}; sigNum < message->get_num_signals(); sigNum++) {
                ICAN_Signal* signal { message->get_signal(sigNum) };

                auto sigId { std::pair{messageId, sigNum} };
                const char* name = "(unknown)";
                auto it { dbc::meta::signalIdToName.find(sigId) };
                if (it != dbc::meta::signalIdToName.end()) name = it->second;

                std::string widgetName { std::string(name) + "###" + dbc::meta::messageIdToName.at(messageId) + name };

                switch (signal->getSignalType()) {
                    case SignalType::INT8:   { auto s = static_cast<CAN_Signal<int8_t>*>(signal);   int8_t   v = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_S8,  &v)) s->set(v); break; }
                    case SignalType::INT16:  { auto s = static_cast<CAN_Signal<int16_t>*>(signal);  int16_t  v = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_S16, &v)) s->set(v); break; }
                    case SignalType::INT32:  { auto s = static_cast<CAN_Signal<int32_t>*>(signal);  int32_t  v = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_S32, &v)) s->set(v); break; }
                    case SignalType::INT64:  { auto s = static_cast<CAN_Signal<int64_t>*>(signal);  int64_t  v = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_S64, &v)) s->set(v); break; }
                    case SignalType::UINT8:  { auto s = static_cast<CAN_Signal<uint8_t>*>(signal);  uint8_t  v = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_U8,  &v)) s->set(v); break; }
                    case SignalType::UINT16: { auto s = static_cast<CAN_Signal<uint16_t>*>(signal); uint16_t v = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_U16, &v)) s->set(v); break; }
                    case SignalType::UINT32: { auto s = static_cast<CAN_Signal<uint32_t>*>(signal); uint32_t v = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_U32, &v)) s->set(v); break; }
                    case SignalType::UINT64: { auto s = static_cast<CAN_Signal<uint64_t>*>(signal); uint64_t v = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_U64, &v)) s->set(v); break; }
                    case SignalType::FLOAT:  { auto s = static_cast<CAN_Signal<float>*>(signal);    float    v = s->get(); if (ImGui::InputFloat(widgetName.c_str(),                    &v)) s->set(v); break; }
                    case SignalType::BOOL:   { auto s = static_cast<CAN_Signal<bool>*>(signal);     bool     v = s->get(); if (ImGui::Checkbox(widgetName.c_str(),                       &v)) s->set(v); break; }
                }
            }
        }
    }

    ImGui::End();
}

uint32_t CAN_IMGUI::time_ms() {
    return 0;
} // do nothing

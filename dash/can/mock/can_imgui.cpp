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
    okay::Option<CAN_IMGUI::MessageChangeInfo> changeInfoOpt { drawUI() };
    if (changeInfoOpt.isNone()) return false;

    MessageChangeInfo info { changeInfoOpt.value() };

    ICAN_Message* message { dbc::driveBus.get_message_from_id(info.messageID) };
    ICAN_Signal* signal { message->get_signal(info.signalNum) };

    switch (info.changedSignal.type) {
        case SignalType::INT8:    signal->set(info.changedSignal.type, &info.changedSignal.value.s8);   break;
        case SignalType::INT16:   signal->set(info.changedSignal.type, &info.changedSignal.value.s16);  break;
        case SignalType::INT32:   signal->set(info.changedSignal.type, &info.changedSignal.value.s32);  break;
        case SignalType::INT64:   signal->set(info.changedSignal.type, &info.changedSignal.value.s64);  break;
        case SignalType::UINT8:   signal->set(info.changedSignal.type, &info.changedSignal.value.u8);   break;
        case SignalType::UINT16:  signal->set(info.changedSignal.type, &info.changedSignal.value.u16);  break;
        case SignalType::UINT32:  signal->set(info.changedSignal.type, &info.changedSignal.value.u32);  break;
        case SignalType::UINT64:  signal->set(info.changedSignal.type, &info.changedSignal.value.u64);  break;
        case SignalType::FLOAT:   signal->set(info.changedSignal.type, &info.changedSignal.value.f);    break;
        case SignalType::BOOL:    signal->set(info.changedSignal.type, &info.changedSignal.value.b);    break;
    }

    message->encode_to_frame(msg);
    return false;
}

okay::Option<CAN_IMGUI::MessageChangeInfo> CAN_IMGUI::drawUI() {
    ImGui::Begin("CAN");

    auto result { okay::Option<CAN_IMGUI::MessageChangeInfo>::none() };

    for (ICAN_Message* message : dbc::driveBus.get_messages()) {
        uint32_t messageId { message->get_id().id };

        if (ImGui::CollapsingHeader(dbc::meta::messageIdToName.at(messageId), ImGuiTreeNodeFlags_DefaultOpen)) {
            for (uint8_t sigNum {}; sigNum < message->get_num_signals(); sigNum++) {
                ICAN_Signal* signal { message->get_signal(sigNum) };

                auto sigId { std::pair{messageId, sigNum} };
                const char* name = "(unknown)";
                auto it { dbc::meta::signalIdToName.find(sigId) };
                if (it != dbc::meta::signalIdToName.end()) name = it->second;

                struct SignalInfo sigInfo;
                sigInfo.name = const_cast<char*>(name);
                sigInfo.type = signal->getSignalType();
                bool changed = false;

                std::string widgetName { std::string(name) + "###" + dbc::meta::messageIdToName.at(messageId) + name };

                switch (signal->getSignalType()) {
                    case SignalType::INT8:   { auto s = static_cast<CAN_Signal<int8_t>*>(signal);   sigInfo.value.s8  = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_S8,  &sigInfo.value.s8))  changed = true; break; }
                    case SignalType::INT16:  { auto s = static_cast<CAN_Signal<int16_t>*>(signal);  sigInfo.value.s16 = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_S16, &sigInfo.value.s16)) changed = true; break; }
                    case SignalType::INT32:  { auto s = static_cast<CAN_Signal<int32_t>*>(signal);  sigInfo.value.s32 = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_S32, &sigInfo.value.s32)) changed = true; break; }
                    case SignalType::INT64:  { auto s = static_cast<CAN_Signal<int64_t>*>(signal);  sigInfo.value.s64 = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_S64, &sigInfo.value.s64)) changed = true; break; }
                    case SignalType::UINT8:  { auto s = static_cast<CAN_Signal<uint8_t>*>(signal);  sigInfo.value.u8  = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_U8,  &sigInfo.value.u8))  changed = true; break; }
                    case SignalType::UINT16: { auto s = static_cast<CAN_Signal<uint16_t>*>(signal); sigInfo.value.u16 = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_U16, &sigInfo.value.u16)) changed = true; break; }
                    case SignalType::UINT32: { auto s = static_cast<CAN_Signal<uint32_t>*>(signal); sigInfo.value.u32 = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_U32, &sigInfo.value.u32)) changed = true; break; }
                    case SignalType::UINT64: { auto s = static_cast<CAN_Signal<uint64_t>*>(signal); sigInfo.value.u64 = s->get(); if (ImGui::InputScalar(widgetName.c_str(), ImGuiDataType_U64, &sigInfo.value.u64)) changed = true; break; }
                    case SignalType::FLOAT:  { auto s = static_cast<CAN_Signal<float>*>(signal);    sigInfo.value.f   = s->get(); if (ImGui::InputFloat(widgetName.c_str(),                     &sigInfo.value.f))   changed = true; break; }
                    case SignalType::BOOL:   { auto s = static_cast<CAN_Signal<bool>*>(signal);     sigInfo.value.b   = s->get(); if (ImGui::Checkbox(widgetName.c_str(),                       &sigInfo.value.b))   changed = true; break; }
                }

                if (changed) {
                    result = okay::Option<CAN_IMGUI::MessageChangeInfo>::some({ messageId, sigNum, sigInfo });
                }
            }
        }
    }

    ImGui::End();
    return result;
}

uint32_t CAN_IMGUI::time_ms() {
    return 0;
} // do nothing

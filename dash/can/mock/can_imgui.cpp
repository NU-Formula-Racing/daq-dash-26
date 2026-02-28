#include "can_imgui.hpp"
#include <string.h>
#include "can/can_dbc.hpp"
#include "imgui.h"
#include "nfr_can/CAN_interface.hpp"
#include "okay/core/util/option.hpp"
#include <cstdint>
#include <cstdlib>
#include <unordered_map>

bool CAN_IMGUI::init(const BaudRate baud) {
    for (ICAN_Message* message : dbc::driveBus.get_messages()) {
        uint32_t messageId { message->get_id().id };
        for (uint8_t sigNum {}; sigNum < message->get_num_signals(); sigNum++) {
            auto sigId { std::pair{message->get_id().id, sigNum} };

            char* name;
            auto it { dbc::meta::signalIdToName.find(sigId) };
            if (it != dbc::meta::signalIdToName.end()) {
                name = const_cast<char*>(it->second);
            }
            
            ICAN_Signal* signal { message->get_signal(sigNum) };

            struct SignalInfo sigInfo;
            sigInfo.name = name;
            sigInfo.type = signal->getSignalType();

            switch (sigInfo.type) {
                case SignalType::INT8:    sigInfo.value.s8 = 0;     break;
                case SignalType::INT16:   sigInfo.value.s16 = 0;    break;
                case SignalType::INT32:   sigInfo.value.s32 = 0;    break;
                case SignalType::INT64:   sigInfo.value.s64 = 0;    break;
                case SignalType::UINT8:   sigInfo.value.u8 = 0;     break;
                case SignalType::UINT16:  sigInfo.value.u16 = 0;    break;
                case SignalType::UINT32:  sigInfo.value.u32 = 0;    break;
                case SignalType::UINT64:  sigInfo.value.u64 = 0;    break;
                case SignalType::FLOAT:   sigInfo.value.f = 0;      break;
                case SignalType::BOOL:    sigInfo.value.b = false;  break;
            }

            messageIdToSignalsInfo[messageId].push_back(sigInfo);
        }
    }
    return true;
}

bool CAN_IMGUI::send(const CAN_Frame& msg) {
    return true;
} // do nothing

bool CAN_IMGUI::recv(CAN_Frame& msg) {
    okay::Option<MessageChangeInfo> changeInfoOpt { drawUI() };
    if (changeInfoOpt.isNone()) return false;

    MessageChangeInfo info { changeInfoOpt.value() };

    ICAN_Message *message { dbc::driveBus.get_message_from_id(info.messageID) };

    ICAN_Signal *signal { message->get_signal(info.signalNum) };

    // we need to set the signal to the value in info.changedSigna
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

                SignalInfo& sigInfo { messageIdToSignalsInfo[messageId][sigNum] };
                
                auto logOnEdit { [&](const auto& val) {
                    // if (ImGui::IsItemDeactivatedAfterEdit()) {
                        // okay::Engine.logger.debug("Value for {} changed to: {}", sigInfo.name, val);
                        // std::cout << std::flush;
                    // }
                    result = okay::Option<CAN_IMGUI::MessageChangeInfo>::some({ messageId, sigNum, sigInfo });
                }};
                
                std::string name { sigInfo.name };
                name.append("###", 3);
                name.append(dbc::meta::messageIdToName.at(messageId));
                
                switch (sigInfo.type) {
                    case SignalType::INT8:    ImGui::InputScalar(name.c_str(), ImGuiDataType_S8, &sigInfo.value.s8);    logOnEdit(sigInfo.value.s8);   break;
                    case SignalType::INT16:   ImGui::InputScalar(name.c_str(), ImGuiDataType_S16, &sigInfo.value.s16);  logOnEdit(sigInfo.value.s16);  break;
                    case SignalType::INT32:   ImGui::InputScalar(name.c_str(), ImGuiDataType_S32, &sigInfo.value.s32);  logOnEdit(sigInfo.value.s32);  break;
                    case SignalType::INT64:   ImGui::InputScalar(name.c_str(), ImGuiDataType_S64, &sigInfo.value.s64);  logOnEdit(sigInfo.value.s64);  break;
                    case SignalType::UINT8:   ImGui::InputScalar(name.c_str(), ImGuiDataType_U8, &sigInfo.value.u8);    logOnEdit(sigInfo.value.u8);   break;
                    case SignalType::UINT16:  ImGui::InputScalar(name.c_str(), ImGuiDataType_U16, &sigInfo.value.u16);  logOnEdit(sigInfo.value.u16);  break;
                    case SignalType::UINT32:  ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &sigInfo.value.u32);  logOnEdit(sigInfo.value.u32);  break;
                    case SignalType::UINT64:  ImGui::InputScalar(name.c_str(), ImGuiDataType_U64, &sigInfo.value.u64);  logOnEdit(sigInfo.value.u64);  break;
                    case SignalType::FLOAT:   ImGui::InputFloat(name.c_str(), &sigInfo.value.f);                        logOnEdit(sigInfo.value.f);    break;
                    case SignalType::BOOL:    ImGui::Checkbox(name.c_str(), &sigInfo.value.b);                          logOnEdit(sigInfo.value.b);    break;
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

CAN_IMGUI::SignalInfo* CAN_IMGUI::getSignalInfo(uint32_t msgId, uint8_t sigNum) {
    return &messageIdToSignalsInfo.at(msgId).at(sigNum);
}

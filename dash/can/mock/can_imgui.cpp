#include "can_imgui.hpp"
#include "can/can_dbc.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "nfr_can/CAN_interface.hpp"
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

            messageIdToSignalsInfo[messageId].push_back(sigInfo);
            
            switch (signal->getSignalType()) {
                case SignalType::INT8:
                    sigInfo.value.s8 = 0;
                    break;
                case SignalType::INT16:
                    sigInfo.value.s16 = 0;
                    break;
                case SignalType::INT32:
                    sigInfo.value.s32 = 0;
                    break;
                case SignalType::INT64:
                    sigInfo.value.s64 = 0;
                    break;
                case SignalType::UINT8:
                    sigInfo.value.u8 = 0;
                    break;
                case SignalType::UINT16:
                    sigInfo.value.u16 = 0;
                    break;
                case SignalType::UINT32:
                    sigInfo.value.u32 = 0;
                    break;
                case SignalType::UINT64:
                    sigInfo.value.u64 = 0;
                    break;
                case SignalType::FLOAT:
                    sigInfo.value.f = 0;
                    break;
                case SignalType::BOOL:
                    sigInfo.value.b = false;
                    break;
            }
        }
    }
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
    
    for (ICAN_Message* message : dbc::driveBus.get_messages()) {
        uint32_t messageId = message->get_id().id;

        if (msg._id == messageId) {
            message->decode_from(msg);
        }

        if (ImGui::CollapsingHeader(dbc::meta::messageIdToName.at(messageId), ImGuiTreeNodeFlags_DefaultOpen)) {
            for (uint8_t sigNum {}; sigNum < message->get_num_signals(); sigNum++) {
                auto sigId { std::pair{message->get_id().id, sigNum} };
                
                ICAN_Signal* signal { message->get_signal(sigNum) };
                
                switch (signal->getSignalType()) {
                    case SignalType::INT8:
                        ImGui::InputScalar(messageIdToSignalsInfo[messageId][sigNum].name, ImGuiDataType_S8, &messageIdToSignalsInfo[messageId][sigNum].value.s8);
                        break;
                    case SignalType::INT16:
                        ImGui::InputScalar(messageIdToSignalsInfo[messageId][sigNum].name, ImGuiDataType_S16, &messageIdToSignalsInfo[messageId][sigNum].value.s16);
                        break;
                    case SignalType::INT32:
                        ImGui::InputScalar(messageIdToSignalsInfo[messageId][sigNum].name, ImGuiDataType_S32, &messageIdToSignalsInfo[messageId][sigNum].value.s32);
                        break;
                    case SignalType::INT64:
                        ImGui::InputScalar(messageIdToSignalsInfo[messageId][sigNum].name, ImGuiDataType_S64, &messageIdToSignalsInfo[messageId][sigNum].value.s64);
                        break;
                    case SignalType::UINT8:
                        ImGui::InputScalar(messageIdToSignalsInfo[messageId][sigNum].name, ImGuiDataType_U8, &messageIdToSignalsInfo[messageId][sigNum].value.u8);
                        break;
                    case SignalType::UINT16:
                        ImGui::InputScalar(messageIdToSignalsInfo[messageId][sigNum].name, ImGuiDataType_U16, &messageIdToSignalsInfo[messageId][sigNum].value.u16);
                        break;
                    case SignalType::UINT32:
                        ImGui::InputScalar(messageIdToSignalsInfo[messageId][sigNum].name, ImGuiDataType_U32, &messageIdToSignalsInfo[messageId][sigNum].value.u32);
                        break;
                    case SignalType::UINT64:
                        ImGui::InputScalar(messageIdToSignalsInfo[messageId][sigNum].name, ImGuiDataType_U64, &messageIdToSignalsInfo[messageId][sigNum].value.u64);
                        break;
                    case SignalType::FLOAT:
                        ImGui::InputFloat(messageIdToSignalsInfo[messageId][sigNum].name, &messageIdToSignalsInfo[messageId][sigNum].value.f);
                        break;
                    case SignalType::BOOL:
                        ImGui::Checkbox(messageIdToSignalsInfo[messageId][sigNum].name, &messageIdToSignalsInfo[messageId][sigNum].value.b);
                        break;
                }
            }
        }
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    
    
    return false;
}

uint32_t CAN_IMGUI::time_ms() {
    return 0;
} // do nothing




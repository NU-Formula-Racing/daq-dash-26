#include "can_imgui.hpp"
#include "can/can_dbc.hpp"
#include <string.h>
#include <imgui.h>
#include <nfr_can/CAN_interface.hpp>
#include <okay/core/okay.hpp>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <string_view>


bool CAN_IMGUI::init(const BaudRate baud) {
    for (ICAN_Message* message : dbc::driveBus.get_messages()) {
        uint32_t messageId { message->get_id().id };

        const char* msgName { dbc::meta::messageIdToName.at(messageId) };
        std::string_view nameView { msgName };

        size_t firstUnderscore { nameView.find('_') };
        std::string_view board { (firstUnderscore != std::string::npos) 
                                    ? nameView.substr(0, firstUnderscore) 
                                    : nameView };

        sortedMessages.push_back({board, messageId});
    }

    std::sort(sortedMessages.begin(), sortedMessages.end(), 
        [](const GroupedMessage& a, const GroupedMessage& b) {
            return a.boardName < b.boardName;
        });
    
    return true;
}

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

    timeSinceStartup += okay::Engine.time->deltaTimeMs();
    
    return false;
}

okay::Option<CAN_IMGUI::MessageChangeInfo> CAN_IMGUI::drawUI() {
    static ImGuiTextFilter filter;

    ImGui::Begin("CAN");

    bool filterChanged {};

    ImGui::SetNextItemWidth(-1.0f);
    if (ImGui::InputTextWithHint("##SearchFilter", "Search Signals...", filter.InputBuf, IM_ARRAYSIZE(filter.InputBuf))) {
        filter.Build();
        filterChanged = true;
    }

    ImGui::Separator();

    auto result { okay::Option<CAN_IMGUI::MessageChangeInfo>::none() };

    if (ImGui::BeginTabBar("CAN_Boards")) {
        std::string_view currentBoard {};
        bool tabOpen {};

        bool expandAll { filterChanged };
        bool collapseAll {};

        for (const GroupedMessage& item : sortedMessages) {
            if (item.boardName != currentBoard) {
                if (tabOpen) {
                    ImGui::EndTabItem();
                }
                currentBoard = item.boardName;

                std::string tabName { currentBoard };

                tabOpen = ImGui::BeginTabItem(tabName.c_str());

                if (tabOpen) {
                    expandAll = ImGui::Button("Expand All");
                    ImGui::SameLine();
                    collapseAll = ImGui::Button("Collapse All");
                }
            }

            if (tabOpen) {
                uint32_t messageID { item.messageID };
                ICAN_Message* message { dbc::driveBus.get_message_from_id(messageID) };
                const char* msgName { dbc::meta::messageIdToName.at(messageID) };

                bool showHeader {};

                if (filter.PassFilter(msgName)) {
                    showHeader = true; 
                } else {
                    for (uint8_t sigNum {}; sigNum < message->get_num_signals(); ++sigNum) {
                        auto sigID { std::pair{messageID, sigNum} };
                        auto it { dbc::meta::signalIdToName.find(sigID) };
                        const char* peekName { (it != dbc::meta::signalIdToName.end())
                                                ? it->second
                                                : "(unknown)" };
                        
                        if (filter.PassFilter(peekName)) {
                            showHeader = true;
                            break; 
                        }
                    }
                }

                if (!showHeader) continue;

                if (expandAll) {
                    ImGui::SetNextItemOpen(true, ImGuiCond_Always);
                } else if (collapseAll) {
                    ImGui::SetNextItemOpen(false, ImGuiCond_Always);
                }

                if (ImGui::CollapsingHeader(msgName, ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::PushID(messageID);

                    for (uint8_t sigNum {}; sigNum < message->get_num_signals(); sigNum++) {
                        ICAN_Signal* signal { message->get_signal(sigNum) };

                        auto sigId { std::pair{messageID, sigNum} };
                        const char* name { "(unknown)" };
                        auto it { dbc::meta::signalIdToName.find(sigId) };
                        if (it != dbc::meta::signalIdToName.end()) name = it->second;

                        if (!filter.PassFilter(name) && !filter.PassFilter(msgName)) {
                            continue;
                        }

                        struct SignalInfo sigInfo;
                        sigInfo.name = const_cast<char*>(name);
                        sigInfo.type = signal->getSignalType();
                        bool changed {};

                        ImGui::PushID(sigNum);

                        ImGui::PushItemWidth(-(ImGui::CalcTextSize(name).x + ImGui::GetStyle().ItemInnerSpacing.x));

                        switch (signal->getSignalType()) {
                            case SignalType::INT8:   { auto s = static_cast<CAN_Signal<int8_t>*>(signal);   sigInfo.value.s8  = s->get(); if (ImGui::InputScalar(name, ImGuiDataType_S8,  &sigInfo.value.s8))  changed = true; break; }
                            case SignalType::INT16:  { auto s = static_cast<CAN_Signal<int16_t>*>(signal);  sigInfo.value.s16 = s->get(); if (ImGui::InputScalar(name, ImGuiDataType_S16, &sigInfo.value.s16)) changed = true; break; }
                            case SignalType::INT32:  { auto s = static_cast<CAN_Signal<int32_t>*>(signal);  sigInfo.value.s32 = s->get(); if (ImGui::InputScalar(name, ImGuiDataType_S32, &sigInfo.value.s32)) changed = true; break; }
                            case SignalType::INT64:  { auto s = static_cast<CAN_Signal<int64_t>*>(signal);  sigInfo.value.s64 = s->get(); if (ImGui::InputScalar(name, ImGuiDataType_S64, &sigInfo.value.s64)) changed = true; break; }
                            case SignalType::UINT8:  { auto s = static_cast<CAN_Signal<uint8_t>*>(signal);  sigInfo.value.u8  = s->get(); if (ImGui::InputScalar(name, ImGuiDataType_U8,  &sigInfo.value.u8))  changed = true; break; }
                            case SignalType::UINT16: { auto s = static_cast<CAN_Signal<uint16_t>*>(signal); sigInfo.value.u16 = s->get(); if (ImGui::InputScalar(name, ImGuiDataType_U16, &sigInfo.value.u16)) changed = true; break; }
                            case SignalType::UINT32: { auto s = static_cast<CAN_Signal<uint32_t>*>(signal); sigInfo.value.u32 = s->get(); if (ImGui::InputScalar(name, ImGuiDataType_U32, &sigInfo.value.u32)) changed = true; break; }
                            case SignalType::UINT64: { auto s = static_cast<CAN_Signal<uint64_t>*>(signal); sigInfo.value.u64 = s->get(); if (ImGui::InputScalar(name, ImGuiDataType_U64, &sigInfo.value.u64)) changed = true; break; }
                            case SignalType::FLOAT:  { auto s = static_cast<CAN_Signal<float>*>(signal);    sigInfo.value.f   = s->get(); if (ImGui::InputFloat(name,                     &sigInfo.value.f))   changed = true; break; }
                            case SignalType::BOOL:   { auto s = static_cast<CAN_Signal<bool>*>(signal);     sigInfo.value.b   = s->get(); if (ImGui::Checkbox(name,                       &sigInfo.value.b))   changed = true; break; }
                        }

                        ImGui::PopItemWidth();

                        ImGui::PopID();

                        if (changed) {
                            result = okay::Option<CAN_IMGUI::MessageChangeInfo>::some({ messageID, sigNum, sigInfo });
                        }
                    }

                    ImGui::PopID();
                }
            }
        }

        if (tabOpen) {
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
    return result;
}

uint32_t CAN_IMGUI::time_ms() {
    return timeSinceStartup;
}

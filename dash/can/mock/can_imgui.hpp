#ifndef __CAN_IMGUI_H__
#define __CAN_IMGUI_H__

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <okay/core/util/option.hpp>
#include <nfr_can/CAN_interface.hpp>

class CAN_IMGUI : public ICAN {
private:
    struct SignalInfo {
        char* name;
        SignalType type;
        union {
            int8_t s8;
            int16_t s16;
            int32_t s32;
            int64_t s64;
            uint8_t u8;
            uint16_t u16;
            uint32_t u32;
            uint64_t u64;
            float f;
            bool b;
        } value;
    };

    struct MessageChangeInfo {
        uint32_t messageID;
        uint8_t signalNum;
        SignalInfo changedSignal;
    };

    std::unordered_map<uint32_t, std::vector<struct SignalInfo>> messageIdToSignalsInfo;

public:
    CAN_IMGUI() {}

    bool init(const BaudRate baud) override;
    bool send(const CAN_Frame& msg) override;
    bool recv(CAN_Frame& msg) override;
    okay::Option<struct MessageChangeInfo> drawUI();
    uint32_t time_ms() override;
    std::unordered_map<uint32_t, std::vector<CAN_IMGUI::SignalInfo>>* getMessageIdToSignalsInfo();
};

#endif

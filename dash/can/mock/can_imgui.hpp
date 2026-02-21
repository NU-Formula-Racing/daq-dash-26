#ifndef __CAN_IMGUI_H__
#define __CAN_IMGUI_H__

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <nfr_can/CAN_interface.hpp>

class CAN_IMGUI : public ICAN {
private:
    struct SignalInfo {
        char* name;

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

    std::unordered_map<uint32_t, std::vector<SignalInfo>> messageIdToSignalsInfo;

public:
    CAN_IMGUI() {}

    bool init(const BaudRate baud) override;
    bool send(const CAN_Frame& msg) override;
    bool recv(CAN_Frame& msg) override;
    uint32_t time_ms() override;
};

#endif

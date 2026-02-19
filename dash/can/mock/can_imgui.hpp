#ifndef __CAN_IMGUI_H__
#define __CAN_IMGUI_H__

#include <cstdint>
#include <nfr_can/CAN_interface.hpp>

class CAN_IMGUI : public ICAN {
private:
    struct PDMInfo {
        float batVoltage;
        float batCur;
    };

    PDMInfo pdmInfo {};

public:
    CAN_IMGUI() {}

    bool init(const BaudRate baud) override;
    bool send(const CAN_Frame& msg) override;
    bool recv(CAN_Frame& msg) override;
    uint32_t time_ms() override;
};

#endif

#ifndef __CAN_IMGUI_H__
#define __CAN_IMGUI_H__

#include <cstdint>
#include <nfr_can/CAN_interface.hpp>

class CAN_IMGUI : public ICAN {
private:
    ISpi& _spi;
    IGpio& _cs;
    IClock& _clock;
    uint32_t missCounter;
    uint32_t recvCount;

public:
    CAN_IMGUI(ISpi& spi, IGpio& cs, IClock& clock) : _spi(spi), _cs(cs), _clock(clock) {}

    bool init(const BaudRate baud) override;
    bool send(const CAN_Frame& msg) override;
    bool recv(CAN_Frame& msg) override;
    uint32_t time_ms() override;
};

#endif

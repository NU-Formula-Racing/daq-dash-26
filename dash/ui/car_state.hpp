#ifndef __CAR_STATE_H__
#define __CAR_STATE_H__

#include "style.hpp"

#include <can/can_dbc.hpp>
#include <glm/glm.hpp>

namespace dash {

class CarState {
   public:
    enum class ECUState { IDLE, PRECHARGE, NEUTRAL, DRIVE };

    static bool driveFaultPresent(){
        const int errorCode = 0x03;   
        bool driveFaultError = dbc::frontRightInverterFaultStatus::faultCode->get() == errorCode ||
                              dbc::frontLeftInverterFaultStatus::faultCode->get() == errorCode ||
                              dbc::rearInverterFaultStatus::faultCode->get() == errorCode;
        
        return driveFaultError;
    }

    static bool hardFaultPresent() {

        bool hardFaultError = dbc::bmsStatus::internalfaultSummary->get() != 0;
                        

        bool imdError = !(dbc::bmsStatus::imdState->get());

        if (hardFaultError || imdError) {
            return true;
        }

        return false;
    }

    static std::string getDriveStateString() {
        if (hardFaultPresent())
            return "FAULT";

        switch (dbc::ecuDriveStatus::driveState->get()) {
            case 0:
                return "IDLE";
            case 1:
                return "PRECHARGE";
            case 2:
                return "NEUTRAL";
            case 3:
                return "DRIVE";
            default:
                return "UNKNOWN";
        }
    }

    static glm::vec4 getDriveStateColor() {
        if (hardFaultPresent())
            return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

        switch (dbc::ecuDriveStatus::driveState->get()) {
            case 0:
                return colors::fromHex(0x219EEBFF);
            case 1:
                return colors::fromHex(0xEBCD21FF);
            case 2:
                return colors::northwesternPurple;
            case 3:
                return colors::fromHex(0x38EB21FF);
            default:
                return colors::fromHex(0xFF00FFFF);
        }
    }
};

}  // namespace dash

#endif  // __CAR_STATE_H__

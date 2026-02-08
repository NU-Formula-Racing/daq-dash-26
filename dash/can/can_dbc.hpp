#include "CAN_interface.cpp"
namespace dbc {
CAN_Signal_FLOAT genAmps = MakeSignalSigned(float, 0, 16, 0.01, 0.0, True);
CAN_Signal_FLOAT frontFanAmps = MakeSignalSigned(float, 16, 8, 0.01, 0.0, True);
CAN_Signal_FLOAT rearFanAmps = MakeSignalSigned(float, 24, 8, 0.01, 0.0, True);
CAN_Signal_FLOAT frontPumpAmps = MakeSignalSigned(float, 32, 8, 0.01, 0.0, True);
CAN_Signal_FLOAT rearPumpAmps = MakeSignalSigned(float, 40, 8, 0.01, 0.0, True);
RX_CAN_Message(5) pdmCurrent{driveBus, 0x2A1, genAmps, frontFanAmps, rearFanAmps, frontPumpAmps, rearPumpAmps};

CAN_Signal_FLOAT batVolt = MakeSignalSigned(float, 0, 16, 0.01, 0.0, True);
CAN_Signal_BOOL lowBatVoltWarning = MakeSignalSigned(bool, 16, 8, 1.0, 0.0, True);
CAN_Signal_BOOL dangerBatVoltWarning = MakeSignalSigned(bool, 24, 8, 1.0, 0.0, True);
RX_CAN_Message(3) pdmBatVolt{driveBus, 0x2A2, batVolt, lowBatVoltWarning, dangerBatVoltWarning};

CAN_Signal_INT16 rpm = MakeSignalSigned(int16_t, 0, 16, 1.0, 0.0, True);
CAN_Signal_INT16 motorCurrent = MakeSignalSigned(int16_t, 16, 16, 0.1, 0.0, True);
CAN_Signal_INT16 dcVoltage = MakeSignalSigned(int16_t, 32, 16, 0.1, 0.0, True);
CAN_Signal_INT16 dcCurrent = MakeSignalSigned(int16_t, 48, 16, 0.1, 0.0, True);
RX_CAN_Message(4) frontRightInverterMotorStatus{driveBus, 0x28B, rpm, motorCurrent, dcVoltage, dcCurrent};

CAN_Signal_INT16 igbtTemp = MakeSignalSigned(int16_t, 0, 16, 0.1, 0.0, True);
CAN_Signal_INT16 motorTemp = MakeSignalSigned(int16_t, 16, 16, 0.1, 0.0, True);
RX_CAN_Message(2) frontRightInverterTempStatus{driveBus, 0x28C, igbtTemp, motorTemp};

CAN_Signal_UINT32 ahCharged = MakeSignalExp(uint32_t, 32, 32, 0.0001, 0.0);
RX_CAN_Message(1) frontRightInverterCurrentDraw{driveBus, nan, ahCharged};

CAN_Signal_UINT32 whDrawn = MakeSignalExp(uint32_t, 0, 32, 0.001, 0.0);
CAN_Signal_UINT32 whCharged = MakeSignalExp(uint32_t, 32, 32, 0.001, 0.0);
RX_CAN_Message(2) frontRightInverterPowerDraw{driveBus, 0x28E, whDrawn, whCharged};

CAN_Signal_UINT8 faultCode = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
RX_CAN_Message(1) frontRightInverterFaultStatus{driveBus, 0x28A, faultCode};

RX_CAN_Message(4) frontLeftInverterMotorStatus{driveBus, 0x286, rpm, motorCurrent, dcVoltage, dcCurrent};

RX_CAN_Message(2) frontLeftInverterTempStatus{driveBus, 0x287, igbtTemp, motorTemp};

RX_CAN_Message(1) frontLeftInverterCurrentDraw{driveBus, nan, ahCharged};

RX_CAN_Message(2) frontLeftInverterPowerDraw{driveBus, 0x289, whDrawn, whCharged};

RX_CAN_Message(1) frontLeftInverterFaultStatus{driveBus, 0x285, faultCode};

RX_CAN_Message(4) rearInverterMotorStatus{driveBus, 0x281, rpm, motorCurrent, dcVoltage, dcCurrent};

RX_CAN_Message(2) rearInverterTempStatus{driveBus, 0x282, igbtTemp, motorTemp};

RX_CAN_Message(1) rearInverterCurrentDraw{driveBus, nan, ahCharged};

RX_CAN_Message(2) rearInverterPowerDraw{driveBus, 0x284, whDrawn, whCharged};

RX_CAN_Message(1) rearInverterFaultStatus{driveBus, 0x280, faultCode};

CAN_Signal_INT32 setCurrent = MakeSignalSigned(int32_t, 0, 32, 0.001, 0.0, True);
RX_CAN_Message(1) ecuSetCurrentFrontLeftInverter{driveBus, 0x20D, setCurrent};

CAN_Signal_INT32 setCurrentBrake = MakeSignalSigned(int32_t, 0, 32, 0.001, 0.0, True);
RX_CAN_Message(1) ecuSetCurrentBrakeFrontLeftInverter{driveBus, 0x20E, setCurrentBrake};

RX_CAN_Message(1) ecuSetCurrentFrontRightInverter{driveBus, 0x20F, setCurrent};

RX_CAN_Message(1) ecuSetCurrentBrakeFrontRightInverter{driveBus, 0x210, setCurrentBrake};

RX_CAN_Message(1) ecuSetCurrentRearInverter{driveBus, 0x200, setCurrent};

RX_CAN_Message(1) ecuSetCurrentBrakeRearInverter{driveBus, 0x201, setCurrentBrake};

CAN_Signal_INT16 apps1Throttle = MakeSignalSigned(int16_t, 0, 16, 1.0, 0.0, True);
CAN_Signal_INT16 apps2Throttle = MakeSignalSigned(int16_t, 16, 16, 1.0, 0.0, True);
RX_CAN_Message(2) ecuThrottle{driveBus, 0x202, apps1Throttle, apps2Throttle};

CAN_Signal_INT16 frontBrakePressure = MakeSignalSigned(int16_t, 0, 16, 1.0, 0.0, True);
CAN_Signal_INT16 rearBrakePressure = MakeSignalSigned(int16_t, 16, 16, 1.0, 0.0, True);
CAN_Signal_BOOL brakePressed = MakeSignalSigned(bool, 32, 8, 1.0, 0.0, True);
RX_CAN_Message(3) ecuBrake{driveBus, 0x203, frontBrakePressure, rearBrakePressure, brakePressed};

CAN_Signal_BOOL implausibilityPresent = MakeSignalSigned(bool, 0, 8, 1.0, 0.0, True);
CAN_Signal_BOOL appssDisagreementImp = MakeSignalSigned(bool, 8, 8, 1.0, 0.0, True);
CAN_Signal_BOOL bppcImp = MakeSignalSigned(bool, 16, 8, 1.0, 0.0, True);
CAN_Signal_BOOL brakeInvalidImp = MakeSignalSigned(bool, 24, 8, 1.0, 0.0, True);
CAN_Signal_BOOL appssInvalidImp = MakeSignalSigned(bool, 32, 8, 1.0, 0.0, True);
RX_CAN_Message(5) ecuImplausibility{driveBus, 0x204, implausibilityPresent, appssDisagreementImp, bppcImp, brakeInvalidImp, appssInvalidImp};

CAN_Signal_UINT8 bmsCommand = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
RX_CAN_Message(1) ecuBmsCommandMessage{driveBus, 0x205, bmsCommand};

CAN_Signal_UINT8 driveState = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
RX_CAN_Message(1) ecuDriveStatus{driveBus, 0x206, driveState};

CAN_Signal_BOOL activeAeroState = MakeSignalSigned(bool, 0, 1, 1.0, 0.0, True);
CAN_Signal_INT16 activeAeroPosition = MakeSignalSigned(int16_t, 1, 16, 1.0, 0.0, True);
RX_CAN_Message(2) ecuActiveAeroCommand{driveBus, 0x208, activeAeroState, activeAeroPosition};

CAN_Signal_UINT8 frontPumpDutyCycle = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
CAN_Signal_UINT8 rearPumpDutyCycle = MakeSignalExp(uint8_t, 8, 8, 1.0, 0.0);
CAN_Signal_UINT8 frontFanDutyCycle = MakeSignalExp(uint8_t, 16, 8, 1.0, 0.0);
CAN_Signal_UINT8 rearFanDutyCycle = MakeSignalExp(uint8_t, 24, 8, 1.0, 0.0);
RX_CAN_Message(4) ecuPumpFanCommand{driveBus, 0x209, frontPumpDutyCycle, rearPumpDutyCycle, frontFanDutyCycle, rearFanDutyCycle};

CAN_Signal_UINT8 accelLutIdResponse = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
RX_CAN_Message(1) ecuLutResponse{driveBus, 0x20A, accelLutIdResponse};

CAN_Signal_BOOL igbtTempLimiting = MakeSignalSigned(bool, 0, 1, 1.0, 0.0, True);
CAN_Signal_BOOL batteryTempLimiting = MakeSignalSigned(bool, 1, 1, 1.0, 0.0, True);
CAN_Signal_BOOL motorTempLimiting = MakeSignalSigned(bool, 2, 1, 1.0, 0.0, True);
RX_CAN_Message(3) ecuTempLimitingStatus{driveBus, 0x20B, igbtTempLimiting, batteryTempLimiting, motorTempLimiting};

CAN_Signal_UINT8 torqueStatus = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
RX_CAN_Message(1) ecuTorqueStatus{driveBus, 0x20C, torqueStatus};

CAN_Signal_UINT8 ecuEnableResponse = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
CAN_Signal_UINT8 bmsEnableResponse = MakeSignalExp(uint8_t, 8, 8, 1.0, 0.0);
CAN_Signal_UINT8 pdmEnableResponse = MakeSignalExp(uint8_t, 16, 8, 1.0, 0.0);
RX_CAN_Message(3) cmRequestDrivetrain{driveBus, 0x490, ecuEnableResponse, bmsEnableResponse, pdmEnableResponse};

CAN_Signal_UINT8 dynamicsEnableResponse = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
CAN_Signal_UINT8 frontEnableResponse = MakeSignalExp(uint8_t, 8, 8, 1.0, 0.0);
CAN_Signal_UINT8 telemetryEnableResponse = MakeSignalExp(uint8_t, 16, 8, 1.0, 0.0);
RX_CAN_Message(3) cmRequestDaq{driveBus, 0x491, dynamicsEnableResponse, frontEnableResponse, telemetryEnableResponse};

CAN_Signal_UINT8 blEnableResponse = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
CAN_Signal_UINT8 brEnableResponse = MakeSignalExp(uint8_t, 8, 8, 1.0, 0.0);
CAN_Signal_UINT8 flEnableResponse = MakeSignalExp(uint8_t, 16, 8, 1.0, 0.0);
CAN_Signal_UINT8 frEnableResponse = MakeSignalExp(uint8_t, 24, 8, 1.0, 0.0);
RX_CAN_Message(4) cmRequestWheel{driveBus, 0x492, blEnableResponse, brEnableResponse, flEnableResponse, frEnableResponse};

CAN_Signal_UINT64 counter = MakeSignalExp(uint64_t, 0, 64, 1.0, 0.0);
RX_CAN_Message(1) cmResponseEcu{driveBus, 0x590, counter};

RX_CAN_Message(1) cmResponseBms{driveBus, 0x591, counter};

RX_CAN_Message(1) cmResponsePdm{driveBus, 0x592, counter};

RX_CAN_Message(1) cmResponseDynamics{driveBus, 0x593, counter};

RX_CAN_Message(1) cmResponseFront{driveBus, 0x594, counter};

RX_CAN_Message(1) cmResponseTelemetry{driveBus, 0x595, counter};

RX_CAN_Message(1) cmResponseBl{driveBus, 0x596, counter};

RX_CAN_Message(1) cmResponseBr{driveBus, 0x597, counter};

RX_CAN_Message(1) cmResponseFl{driveBus, 0x598, counter};

RX_CAN_Message(1) cmResponseFr{driveBus, 0x599, counter};

CAN_Signal_UINT8 ecuStatus = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
CAN_Signal_UINT8 bmsStatus = MakeSignalExp(uint8_t, 8, 8, 1.0, 0.0);
CAN_Signal_UINT8 pdmStatus = MakeSignalExp(uint8_t, 16, 8, 1.0, 0.0);
RX_CAN_Message(3) cmNodeStatusDrivetrain{driveBus, 0x192, ecuStatus, bmsStatus, pdmStatus};

CAN_Signal_UINT8 dynamicsStatus = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
CAN_Signal_UINT8 frontStatus = MakeSignalExp(uint8_t, 8, 8, 1.0, 0.0);
CAN_Signal_UINT8 telemetryStatus = MakeSignalExp(uint8_t, 16, 8, 1.0, 0.0);
RX_CAN_Message(3) cmNodeStatusDaq{driveBus, 0x193, dynamicsStatus, frontStatus, telemetryStatus};

CAN_Signal_UINT8 blStatus = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
CAN_Signal_UINT8 brStatus = MakeSignalExp(uint8_t, 8, 8, 1.0, 0.0);
CAN_Signal_UINT8 flStatus = MakeSignalExp(uint8_t, 16, 8, 1.0, 0.0);
CAN_Signal_UINT8 frStatus = MakeSignalExp(uint8_t, 24, 8, 1.0, 0.0);
RX_CAN_Message(4) cmNodeStatusWheel{driveBus, 0x194, blStatus, brStatus, flStatus, frStatus};

CAN_Signal_FLOAT maxDischargeCurrent = MakeSignalSigned(float, 0, 12, 0.1, 0.0, True);
CAN_Signal_FLOAT maxRegenCurrent = MakeSignalSigned(float, 12, 12, 0.1, 0.0, True);
CAN_Signal_FLOAT batteryVoltage = MakeSignalSigned(float, 24, 16, 0.01, 0.0, True);
CAN_Signal_FLOAT batteryTemperature = MakeSignalSigned(float, 40, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT batteryCurrent = MakeSignalSigned(float, 48, 16, 0.01, -100.0, True);
RX_CAN_Message(5) bmsSoe{driveBus, 0x150, maxDischargeCurrent, maxRegenCurrent, batteryVoltage, batteryTemperature, batteryCurrent};

CAN_Signal_BOOL internalfaultSummary = MakeSignalSigned(bool, 0, 1, 1.0, 0.0, True);
CAN_Signal_BOOL externalFault = MakeSignalSigned(bool, 1, 1, 1.0, 0.0, True);
CAN_Signal_BOOL undervoltageFault = MakeSignalSigned(bool, 2, 1, 1.0, 0.0, True);
CAN_Signal_BOOL overvoltageFault = MakeSignalSigned(bool, 3, 1, 1.0, 0.0, True);
CAN_Signal_BOOL undertemperatureFault = MakeSignalSigned(bool, 4, 1, 1.0, 0.0, True);
CAN_Signal_BOOL overtemperatureFault = MakeSignalSigned(bool, 5, 1, 1.0, 0.0, True);
CAN_Signal_BOOL overcurrentFault = MakeSignalSigned(bool, 6, 1, 1.0, 0.0, True);
CAN_Signal_BOOL openWireFault = MakeSignalSigned(bool, 7, 1, 1.0, 0.0, True);
CAN_Signal_BOOL openWireTempFault = MakeSignalSigned(bool, 8, 1, 1.0, 0.0, True);
CAN_Signal_BOOL pecFault = MakeSignalSigned(bool, 9, 1, 1.0, 0.0, True);
CAN_Signal_UINT8 totalPecFailures = MakeSignalExp(uint8_t, 10, 8, 1.0, 0.0);
RX_CAN_Message(11) bmsFaults{driveBus, 0x151, internalfaultSummary, externalFault, undervoltageFault, overvoltageFault, undertemperatureFault, overtemperatureFault, overcurrentFault, openWireFault, openWireTempFault, pecFault, totalPecFailures};

CAN_Signal_UINT8 bmsState = MakeSignalExp(uint8_t, 0, 8, 1.0, 0.0);
CAN_Signal_UINT8 imdState = MakeSignalExp(uint8_t, 8, 8, 1.0, 0.0);
CAN_Signal_FLOAT maxCellTemp = MakeSignalSigned(float, 16, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT minCellTemp = MakeSignalSigned(float, 24, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT maxCellVoltage = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT minCellVoltage = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT bmsSoc = MakeSignalSigned(float, 48, 8, 0.004, 0.0, True);
RX_CAN_Message(7) bmsStatus{driveBus, 0x152, bmsState, imdState, maxCellTemp, minCellTemp, maxCellVoltage, minCellVoltage, bmsSoc};

CAN_Signal_FLOAT cellV0 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV1 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV2 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV3 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV4 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV5 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV6 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset0 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages0{driveBus, 0x153, cellV0, cellV1, cellV2, cellV3, cellV4, cellV5, cellV6, cellOcvOffset0};

CAN_Signal_FLOAT cellV7 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV8 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV9 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV10 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV11 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV12 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV13 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset1 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages1{driveBus, 0x154, cellV7, cellV8, cellV9, cellV10, cellV11, cellV12, cellV13, cellOcvOffset1};

CAN_Signal_FLOAT cellV14 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV15 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV16 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV17 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV18 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV19 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV20 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset2 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages2{driveBus, 0x155, cellV14, cellV15, cellV16, cellV17, cellV18, cellV19, cellV20, cellOcvOffset2};

CAN_Signal_FLOAT cellV21 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV22 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV23 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV24 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV25 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV26 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV27 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset3 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages3{driveBus, 0x156, cellV21, cellV22, cellV23, cellV24, cellV25, cellV26, cellV27, cellOcvOffset3};

CAN_Signal_FLOAT cellV28 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV29 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV30 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV31 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV32 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV33 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV34 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset4 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages4{driveBus, 0x157, cellV28, cellV29, cellV30, cellV31, cellV32, cellV33, cellV34, cellOcvOffset4};

CAN_Signal_FLOAT cellV35 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV36 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV37 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV38 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV39 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV40 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV41 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset5 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages5{driveBus, 0x158, cellV35, cellV36, cellV37, cellV38, cellV39, cellV40, cellV41, cellOcvOffset5};

CAN_Signal_FLOAT cellV42 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV43 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV44 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV45 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV46 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV47 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV48 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset6 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages6{driveBus, 0x159, cellV42, cellV43, cellV44, cellV45, cellV46, cellV47, cellV48, cellOcvOffset6};

CAN_Signal_FLOAT cellV49 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV50 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV51 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV52 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV53 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV54 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV55 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset7 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages7{driveBus, 0x15A, cellV49, cellV50, cellV51, cellV52, cellV53, cellV54, cellV55, cellOcvOffset7};

CAN_Signal_FLOAT cellV56 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV57 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV58 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV59 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV60 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV61 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV62 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset8 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages8{driveBus, 0x15B, cellV56, cellV57, cellV58, cellV59, cellV60, cellV61, cellV62, cellOcvOffset8};

CAN_Signal_FLOAT cellV63 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV64 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV65 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV66 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV67 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV68 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV69 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset9 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages9{driveBus, 0x15C, cellV63, cellV64, cellV65, cellV66, cellV67, cellV68, cellV69, cellOcvOffset9};

CAN_Signal_FLOAT cellV70 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV71 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV72 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV73 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV74 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV75 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV76 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset10 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages10{driveBus, 0x15D, cellV70, cellV71, cellV72, cellV73, cellV74, cellV75, cellV76, cellOcvOffset10};

CAN_Signal_FLOAT cellV77 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV78 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV79 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV80 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV81 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV82 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV83 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset11 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages11{driveBus, 0x15E, cellV77, cellV78, cellV79, cellV80, cellV81, cellV82, cellV83, cellOcvOffset11};

CAN_Signal_FLOAT cellV84 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV85 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV86 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV87 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV88 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV89 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV90 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset12 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages12{driveBus, 0x15F, cellV84, cellV85, cellV86, cellV87, cellV88, cellV89, cellV90, cellOcvOffset12};

CAN_Signal_FLOAT cellV91 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV92 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV93 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV94 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV95 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV96 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV97 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset13 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages13{driveBus, 0x160, cellV91, cellV92, cellV93, cellV94, cellV95, cellV96, cellV97, cellOcvOffset13};

CAN_Signal_FLOAT cellV98 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV99 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV100 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV101 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV102 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV103 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV104 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset14 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages14{driveBus, 0x161, cellV98, cellV99, cellV100, cellV101, cellV102, cellV103, cellV104, cellOcvOffset14};

CAN_Signal_FLOAT cellV105 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV106 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV107 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV108 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV109 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV110 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV111 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset15 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages15{driveBus, 0x162, cellV105, cellV106, cellV107, cellV108, cellV109, cellV110, cellV111, cellOcvOffset15};

CAN_Signal_FLOAT cellV112 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV113 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV114 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV115 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV116 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV117 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV118 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset16 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages16{driveBus, 0x163, cellV112, cellV113, cellV114, cellV115, cellV116, cellV117, cellV118, cellOcvOffset16};

CAN_Signal_FLOAT cellV119 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV120 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV121 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV122 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV123 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV124 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV125 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset17 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages17{driveBus, 0x164, cellV119, cellV120, cellV121, cellV122, cellV123, cellV124, cellV125, cellOcvOffset17};

CAN_Signal_FLOAT cellV126 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV127 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV128 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV129 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV130 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV131 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV132 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset18 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages18{driveBus, 0x165, cellV126, cellV127, cellV128, cellV129, cellV130, cellV131, cellV132, cellOcvOffset18};

CAN_Signal_FLOAT cellV133 = MakeSignalSigned(float, 0, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV134 = MakeSignalSigned(float, 8, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV135 = MakeSignalSigned(float, 16, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV136 = MakeSignalSigned(float, 24, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV137 = MakeSignalSigned(float, 32, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV138 = MakeSignalSigned(float, 40, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellV139 = MakeSignalSigned(float, 48, 8, 0.012, 2.0, True);
CAN_Signal_FLOAT cellOcvOffset19 = MakeSignalSigned(float, 56, 8, 0.004, 0.0, True);
RX_CAN_Message(8) bmsVoltages19{driveBus, 0x166, cellV133, cellV134, cellV135, cellV136, cellV137, cellV138, cellV139, cellOcvOffset19};

CAN_Signal_FLOAT cellT0 = MakeSignalSigned(float, 0, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT1 = MakeSignalSigned(float, 8, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT2 = MakeSignalSigned(float, 16, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT3 = MakeSignalSigned(float, 24, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT4 = MakeSignalSigned(float, 32, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT5 = MakeSignalSigned(float, 40, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT6 = MakeSignalSigned(float, 48, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT7 = MakeSignalSigned(float, 56, 8, 1.0, -40.0, True);
RX_CAN_Message(8) bmsTemperatures0{driveBus, 0x167, cellT0, cellT1, cellT2, cellT3, cellT4, cellT5, cellT6, cellT7};

CAN_Signal_FLOAT cellT8 = MakeSignalSigned(float, 0, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT9 = MakeSignalSigned(float, 8, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT10 = MakeSignalSigned(float, 16, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT11 = MakeSignalSigned(float, 24, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT12 = MakeSignalSigned(float, 32, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT13 = MakeSignalSigned(float, 40, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT14 = MakeSignalSigned(float, 48, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT15 = MakeSignalSigned(float, 56, 8, 1.0, -40.0, True);
RX_CAN_Message(8) bmsTemperatures1{driveBus, 0x168, cellT8, cellT9, cellT10, cellT11, cellT12, cellT13, cellT14, cellT15};

CAN_Signal_FLOAT cellT16 = MakeSignalSigned(float, 0, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT17 = MakeSignalSigned(float, 8, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT18 = MakeSignalSigned(float, 16, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT19 = MakeSignalSigned(float, 24, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT20 = MakeSignalSigned(float, 32, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT21 = MakeSignalSigned(float, 40, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT22 = MakeSignalSigned(float, 48, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT23 = MakeSignalSigned(float, 56, 8, 1.0, -40.0, True);
RX_CAN_Message(8) bmsTemperatures2{driveBus, 0x169, cellT16, cellT17, cellT18, cellT19, cellT20, cellT21, cellT22, cellT23};

CAN_Signal_FLOAT cellT24 = MakeSignalSigned(float, 0, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT25 = MakeSignalSigned(float, 8, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT26 = MakeSignalSigned(float, 16, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT27 = MakeSignalSigned(float, 24, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT28 = MakeSignalSigned(float, 32, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT29 = MakeSignalSigned(float, 40, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT30 = MakeSignalSigned(float, 48, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT31 = MakeSignalSigned(float, 56, 8, 1.0, -40.0, True);
RX_CAN_Message(8) bmsTemperatures3{driveBus, 0x16A, cellT24, cellT25, cellT26, cellT27, cellT28, cellT29, cellT30, cellT31};

CAN_Signal_FLOAT cellT32 = MakeSignalSigned(float, 0, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT33 = MakeSignalSigned(float, 8, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT34 = MakeSignalSigned(float, 16, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT35 = MakeSignalSigned(float, 24, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT36 = MakeSignalSigned(float, 32, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT37 = MakeSignalSigned(float, 40, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT38 = MakeSignalSigned(float, 48, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT39 = MakeSignalSigned(float, 56, 8, 1.0, -40.0, True);
RX_CAN_Message(8) bmsTemperatures4{driveBus, 0x16B, cellT32, cellT33, cellT34, cellT35, cellT36, cellT37, cellT38, cellT39};

CAN_Signal_FLOAT cellT40 = MakeSignalSigned(float, 0, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT41 = MakeSignalSigned(float, 8, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT42 = MakeSignalSigned(float, 16, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT43 = MakeSignalSigned(float, 24, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT44 = MakeSignalSigned(float, 32, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT45 = MakeSignalSigned(float, 40, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT46 = MakeSignalSigned(float, 48, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT47 = MakeSignalSigned(float, 56, 8, 1.0, -40.0, True);
RX_CAN_Message(8) bmsTemperatures5{driveBus, 0x16C, cellT40, cellT41, cellT42, cellT43, cellT44, cellT45, cellT46, cellT47};

CAN_Signal_FLOAT cellT48 = MakeSignalSigned(float, 0, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT49 = MakeSignalSigned(float, 8, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT50 = MakeSignalSigned(float, 16, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT51 = MakeSignalSigned(float, 24, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT52 = MakeSignalSigned(float, 32, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT53 = MakeSignalSigned(float, 40, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT54 = MakeSignalSigned(float, 48, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT55 = MakeSignalSigned(float, 56, 8, 1.0, -40.0, True);
RX_CAN_Message(8) bmsTemperatures6{driveBus, 0x16D, cellT48, cellT49, cellT50, cellT51, cellT52, cellT53, cellT54, cellT55};

CAN_Signal_FLOAT cellT56 = MakeSignalSigned(float, 0, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT57 = MakeSignalSigned(float, 8, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT58 = MakeSignalSigned(float, 16, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT59 = MakeSignalSigned(float, 24, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT60 = MakeSignalSigned(float, 32, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT61 = MakeSignalSigned(float, 40, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT62 = MakeSignalSigned(float, 48, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT63 = MakeSignalSigned(float, 56, 8, 1.0, -40.0, True);
RX_CAN_Message(8) bmsTemperatures7{driveBus, 0x16E, cellT56, cellT57, cellT58, cellT59, cellT60, cellT61, cellT62, cellT63};

CAN_Signal_FLOAT cellT64 = MakeSignalSigned(float, 0, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT65 = MakeSignalSigned(float, 8, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT66 = MakeSignalSigned(float, 16, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT67 = MakeSignalSigned(float, 24, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT68 = MakeSignalSigned(float, 32, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT69 = MakeSignalSigned(float, 40, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT70 = MakeSignalSigned(float, 48, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT71 = MakeSignalSigned(float, 56, 8, 1.0, -40.0, True);
RX_CAN_Message(8) bmsTemperatures8{driveBus, 0x16F, cellT64, cellT65, cellT66, cellT67, cellT68, cellT69, cellT70, cellT71};

CAN_Signal_FLOAT cellT72 = MakeSignalSigned(float, 0, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT73 = MakeSignalSigned(float, 8, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT74 = MakeSignalSigned(float, 16, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT75 = MakeSignalSigned(float, 24, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT76 = MakeSignalSigned(float, 32, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT77 = MakeSignalSigned(float, 40, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT78 = MakeSignalSigned(float, 48, 8, 1.0, -40.0, True);
CAN_Signal_FLOAT cellT79 = MakeSignalSigned(float, 56, 8, 1.0, -40.0, True);
RX_CAN_Message(8) bmsTemperatures9{driveBus, 0x170, cellT72, cellT73, cellT74, cellT75, cellT76, cellT77, cellT78, cellT79};

CAN_Signal_UINT64 controllerError = MakeSignalExp(uint64_t, 0, 64, 1.0, 0.0);
RX_CAN_Message(1) can2usbControllerError{driveBus, 0x004, controllerError};

CAN_Signal_UINT64 controllerErrorNack = MakeSignalExp(uint64_t, 0, 64, 1.0, 0.0);
RX_CAN_Message(1) can2usbControllerErrorNackError{driveBus, 0x024, controllerErrorNack};

CAN_Signal_UINT64 controllerErrorProtocolViolation = MakeSignalExp(uint64_t, 0, 64, 1.0, 0.0);
RX_CAN_Message(1) can2usbControllerErrorProtocolViolation{driveBus, 0x00C, controllerErrorProtocolViolation};

}
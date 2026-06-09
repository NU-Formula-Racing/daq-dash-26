#include "imu_pdm_tlm_debug_page.hpp"

#include "okay/core/ui/builder.hpp"
#include "okay/core/ui/element.hpp"
#include "page.hpp"
#include "shared_elements.hpp"
#include "style.hpp"

#include <okay/okay.hpp>

namespace ui = okay::ui;

namespace dash {

void IMUPDMTLMDebugPage::initializePage() {
    okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
        SharedElements::get().skyboxMaterial);
    okay::UIStyle::main().setMainFont(*fonts::latoBold);

    _entities = {
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 3),
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 3),
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 2),
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildLaunchControlIndicator), 1),
        okay::ecs::uiEntity(BIND_TO_THIS(buildIMUPDMTLMDebug), 3),
    };
}

void IMUPDMTLMDebugPage::closePage() {
    for (okay::ECSEntity& entity : _entities) {
        entity.destroy();
    }
}

okay::UIElement IMUPDMTLMDebugPage::buildIMUPDMTLMDebug() {
    // clang-format off
    return ui::relFrame(0.0f, 0.0f, 1.0f, 1.0f)
        .axisSet(okay::UIAxis::Vertical)(
            ui::spacer(),
            ui::box()
                .axisSet(okay::UIAxis::Horizontal)
                .childSpacingSet(10)
                .widthGrow()
                .marginSet(10)
                .bottomMarginSet(20) (
                    ui::spacer(),
                    buildContainer()(
                        ui::h1("IMU"), // both front brokers
                        buildIMU1()
                    ),
                    buildContainer()(
                        ui::h1("IMU (CONT.)"), // both back brokers
                        buildIMU2()
                    ),
                    buildContainer()(
                        ui::h1("TELEMETRY"),
                        buildTelemetry()
                    ),
                    buildContainer()(
                        ui::h1("PDM"),
                        buildPDM()
                    ),
                    ui::spacer()
                ),
            ui::spacer()
        );
    // clang-format on
}

okay::UIElement IMUPDMTLMDebugPage::buildContainer() {
    return ui::slot()
        .widthFixed(180)
        .heightFixed(350)
        .axisSet(okay::UIAxis::Vertical)
        .borderColorSet(colors::northwesternPurple)
        .borderRadiusSet(10)
        .borderWidthSet(5)
        .paddingSet(10)
        .backgroundColorSet(glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
}

okay::UIElement IMUPDMTLMDebugPage::buildIMU1() {
    return ui::growbox(okay::UIAxis::Vertical)(ui::h2("IMU"),
        keyValuePair("X Accel", dbc::imuAcceleration::xAxisAcceleration->get()),
        keyValuePair("Y Accel", dbc::imuAcceleration::yAxisAcceleration->get()),
        keyValuePair("Z Accel", dbc::imuAcceleration::zAxisAcceleration->get()),

        keyValuePair("Yaw", dbc::imuYawPitchRoll::yaw->get()),
        keyValuePair("Pitch", dbc::imuYawPitchRoll::pitch->get()),
        keyValuePair("Roll", dbc::imuYawPitchRoll::roll->get()),

        keyValuePair("X Angular Rate", dbc::imuAngularRate::xAxisAngularRate->get()),
        keyValuePair("Y Angular Rate", dbc::imuAngularRate::yAxisAngularRate->get()),
        keyValuePair("Z Angular Rate", dbc::imuAngularRate::zAxisAngularRate->get()),

        keyValuePair("Pos Latitude", dbc::imuPositionIns::positionLatitude->get()),
        keyValuePair("Pos Longitude", dbc::imuPositionIns::positionLongitude->get()),
        keyValuePair("Pos Altutude", dbc::imuPositionIns::positionAltutude->get()),

        keyValuePair("X Velocity", dbc::imuVelocity::xAxisVelocity->get()),
        keyValuePair("Y Velocity", dbc::imuVelocity::yAxisVelocity->get()),
        keyValuePair("Z Velocity", dbc::imuVelocity::zAxisVelocity->get()),

        keyValuePair("X Magnetometer", dbc::imuMag::xAxisMagnetometer->get()),
        keyValuePair("Y Magnetometer", dbc::imuMag::yAxisMagnetometer->get()),
        keyValuePair("Z Magnetometer", dbc::imuMag::zAxisMagnetometer->get()),

        keyValuePair("Temperature", dbc::imuPresTemp::temperature->get()));
}

okay::UIElement IMUPDMTLMDebugPage::buildIMU2() {
    return ui::growbox(okay::UIAxis::Vertical)(ui::h2("IMU (cont'd)"),
        keyValuePair("Pressure", dbc::imuPresTemp::pressure->get()),

        keyValuePair("No G X Accel", dbc::imuAccelerationNoG::noGXAxisAcceleration->get()),
        keyValuePair("No G Y Accel", dbc::imuAccelerationNoG::noGYAxisAcceleration->get()),
        keyValuePair("No G Z Accel", dbc::imuAccelerationNoG::noGZAxisAcceleration->get()),

        keyValuePair("INS Mode", dbc::imuInsStatus::insMode->get()),
        keyValuePair("GNSS Fix", dbc::imuInsStatus::gnssFix->get()),
        keyValuePair("INS Error", dbc::imuInsStatus::insError->get()),
        keyValuePair("GNSS Heading INS", dbc::imuInsStatus::gnssHeadingIns->get()),
        keyValuePair("GNSS Compass", dbc::imuInsStatus::gnssCompass->get()),

        keyValuePair("UTC Year", dbc::imuUtcTime::utcYear->get()),
        keyValuePair("UTC Month", dbc::imuUtcTime::utcMonth->get()),
        keyValuePair("UTC Day", dbc::imuUtcTime::utcDay->get()),
        keyValuePair("UTC Hour", dbc::imuUtcTime::utcHour->get()),
        keyValuePair("UTC Minutes", dbc::imuUtcTime::utcMinutes->get()),
        keyValuePair("UTC Seconds", dbc::imuUtcTime::utcSeconds->get()),
        keyValuePair("UTC Millisec", dbc::imuUtcTime::utcMilliseconds->get()),

        keyValuePair("X Delta Velocity", dbc::imuDeltaVel::xAxisDeltaVelocity->get()),
        keyValuePair("Y Delta Velocity", dbc::imuDeltaVel::yAxisDeltaVelocity->get()),
        keyValuePair("Z Delta Velocity", dbc::imuDeltaVel::zAxisDeltaVelocity->get()));
}

okay::UIElement IMUPDMTLMDebugPage::buildTelemetry() {
    return ui::growbox(okay::UIAxis::Vertical)(ui::h2("Telemetry"),
        keyValuePair("RTC Hour", dbc::telemetryRtcTime::rtcHour->get()),
        keyValuePair("RTC Minute", dbc::telemetryRtcTime::rtcMinute->get()),
        keyValuePair("RTC Second", dbc::telemetryRtcTime::rtcSecond->get()),
        keyValuePair("RTC Subsecond", dbc::telemetryRtcTime::rtcSubsecond->get()),

        keyValuePair("RTC Year", dbc::telemetryRtcDate::rtcYear->get()),
        keyValuePair("RTC Month", dbc::telemetryRtcDate::rtcMonth->get()),
        keyValuePair("RTC Day", dbc::telemetryRtcDate::rtcDay->get()),
        keyValuePair("RTC Weekday", dbc::telemetryRtcDate::rtcWeekday->get()),
        keyValuePair("Miles Driven", dbc::telemetryOdometer::milesDriven->get()),

        keyValuePair("Log File", dbc::telemetryStatus::logFile->get()));
}

okay::UIElement IMUPDMTLMDebugPage::buildPDM() {
    return ui::growbox(okay::UIAxis::Vertical)(ui::h2("PDM"),
        keyValuePair("Gen Amps", dbc::pdmCurrent::genAmps->get()),
        keyValuePair("Front Fan Amps", dbc::pdmCurrent::frontFanAmps->get()),
        keyValuePair("Rear Fan Amps", dbc::pdmCurrent::rearFanAmps->get()),
        keyValuePair("Front Pump Amps", dbc::pdmCurrent::frontPumpAmps->get()),
        keyValuePair("Rear Pump Amps", dbc::pdmCurrent::rearPumpAmps->get()),

        keyValuePair("Bat Volt", dbc::pdmBatVolt::batVolt->get()),
        keyValuePair("Low Bat Volt Warn", dbc::pdmBatVolt::lowBatVoltWarning->get()),
        keyValuePair("Danger Bat Volt Warn", dbc::pdmBatVolt::dangerBatVoltWarning->get()),

        keyValuePair("Coolant 1 Volt", dbc::pdmCoolant::coolant1Volt->get()),
        keyValuePair("Coolant 2 Volt", dbc::pdmCoolant::coolant2Volt->get()),
        keyValuePair("Coolant 3 Volt", dbc::pdmCoolant::coolant3Volt->get()));
}

};  // namespace dash

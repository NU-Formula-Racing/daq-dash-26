#ifndef __DAQ_DEBUG_PAGE_H__
#define __DAQ_DEBUG_PAGE_H__

#include "okay/core/ui/builder.hpp"
#include "okay/core/ui/element.hpp"
#include "page.hpp"
#include "shared_elements.hpp"
#include "style.hpp"

#include <okay/okay.hpp>

#include <can/can_dbc.hpp>
#include <memory>

namespace ui = okay::ui;


#ifndef BIND_TO_THIS
#define BIND_TO_THIS(fnName)   \
    [this]() {                 \
        return this->fnName(); \
    }
#endif


namespace dash {

class DAQDebugPage : public IPage {
   public:
    DAQDebugPage() {}

    void initializePage() {
        okay::Engine.logger.debug("Creating entities for DAQ/LV Debug page!");

        okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
            SharedElements::get().skyboxMaterial);
        okay::UIStyle::main().setMainFont(*fonts::latoBold);

        _entities = {
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 1),
            okay::ecs::uiEntity(BIND_TO_THIS(buildDAQDebug), 3),
        };
    }

    okay::UIElement buildDAQDebug() {
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
                            ui::h1("FRONT BROKERS"), // both front brokers
                            buildFrontBrokers()
                        ),
                        buildContainer()(
                            ui::h1("BACK BROKERS"), // both back brokers
                            buildBackBrokers()
                        ),
                        buildContainer()(
                            ui::h1("IMU"),
                            buildIMU()
                        ),
                        buildContainer()(
                            ui::h1("TELEMETRY"),
                            buildTelemetry()
                        ),
                        ui::spacer()
                    ),
                ui::spacer()
            );
        // clang-format on
    }

    okay::UIElement buildContainer() {
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

    okay::UIElement buildFrontBrokers() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
            ui::h2("Front Left Broker"),
            keyValuePair(
                "FL Tire Temp 0",
                dbc::flBrokerTemp1::flTireTemp0->get()
            ),
            keyValuePair(
                "FL Tire Temp 1",
                dbc::flBrokerTemp1::flTireTemp1->get()
            ),
            keyValuePair(
                "FL Tire Temp 2",
                dbc::flBrokerTemp1::flTireTemp2->get()
            ),
            keyValuePair(
                "FL Tire Temp 3",
                dbc::flBrokerTemp1::flTireTemp3->get()
            ),
            keyValuePair(
                "FL Tire Temp 4",
                dbc::flBrokerTemp2::flTireTemp4->get()
            ),
            keyValuePair(
                "FL Tire Temp 5",
                dbc::flBrokerTemp2::flTireTemp5->get()
            ),
            keyValuePair(
                "FL Tire Temp 6",
                dbc::flBrokerTemp2::flTireTemp6->get()
            ),
            keyValuePair(
                "FL Tire Temp 7",
                dbc::flBrokerTemp2::flTireTemp7->get()
            ),
            keyValuePair(
                "FL Strain Gauge Uncalibrated",
                dbc::flBrokerSusUncalibrated::flStrainGaugeUncalibrated->get()
            ),
            keyValuePair(
                "FL Sus Pot Uncalibrated",
                dbc::flBrokerSusUncalibrated::flSusPotUncalibrated->get()
            ),
            keyValuePair(
                "FL Strain Gauge Error",
                dbc::flBrokerCanErrorMsg::flStrainGaugeError->get()
            ),
            keyValuePair(
                "FL Sus Pot Error",
                dbc::flBrokerCanErrorMsg::flSusPotError->get()
            ),
            keyValuePair(
                "FL Tire Temp Error",
                dbc::flBrokerCanErrorMsg::flTireTempError->get()
            ),
            keyValuePair(
                "FL Heartbeat Count",
                dbc::flBrokerCanErrorMsg::flHeartbeatCount->get()
            ),
            keyValuePair(
                "FL Strain Gauge Calibrated",
                dbc::flBrokerSusCalibrated::flStrainGaugeCalibrated->get()
            ),
            keyValuePair(
                "FL Sus Pot Calibrated",
                dbc::flBrokerSusCalibrated::flSusPotCalibrated->get()
            ),

            ui::vspacer(10),
            ui::h2("Front Right Broker"),
            keyValuePair(
                "FR Tire Temp 0",
                dbc::frBrokerTemp1::frTireTemp0->get()
            ),
            keyValuePair(
                "FR Tire Temp 1",
                dbc::frBrokerTemp1::frTireTemp1->get()
            ),
            keyValuePair(
                "FR Tire Temp 2",
                dbc::frBrokerTemp1::frTireTemp2->get()
            ),
            keyValuePair(
                "FR Tire Temp 3",
                dbc::frBrokerTemp1::frTireTemp3->get()
            ),
            keyValuePair(
                "FR Tire Temp 4",
                dbc::frBrokerTemp2::frTireTemp4->get()
            ),
            keyValuePair(
                "FR Tire Temp 5",
                dbc::frBrokerTemp2::frTireTemp5->get()
            ),
            keyValuePair(
                "FR Tire Temp 6",
                dbc::frBrokerTemp2::frTireTemp6->get()
            ),
            keyValuePair(
                "FR Tire Temp 7",
                dbc::frBrokerTemp2::frTireTemp7->get()
            ),
            keyValuePair(
                "FR Strain Gauge Uncalibrated",
                dbc::frBrokerSusUncalibrated::frStrainGaugeUncalibrated->get()
            ),
            keyValuePair(
                "FR Sus Pot Uncalibrated",
                dbc::frBrokerSusUncalibrated::frSusPotUncalibrated->get()
            ),
            keyValuePair(
                "FR Strain Gauge Error",
                dbc::frBrokerCanErrorMsg::frStrainGaugeError->get()
            ),
            keyValuePair(
                "FR Sus Pot Error",
                dbc::frBrokerCanErrorMsg::frSusPotError->get()
            ),
            keyValuePair(
                "FR Tire Temp Error",
                dbc::frBrokerCanErrorMsg::frTireTempError->get()
            ),
            keyValuePair(
                "FR Heartbeat Count",
                dbc::frBrokerCanErrorMsg::frHeartbeatCount->get()
            ),
            keyValuePair(
                "FR Strain Gauge Calibrated",
                dbc::frBrokerSusCalibrated::frStrainGaugeCalibrated->get()
            ),
            keyValuePair(
                "FR Sus Pot Calibrated",
                dbc::frBrokerSusCalibrated::frSusPotCalibrated->get()
            )   
        );
        // clang-format off
    }

    okay::UIElement buildBackBrokers() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
            ui::h2("Back Left Broker"),
            keyValuePair(
                "BL Tire Temp 0",
                dbc::blBrokerTemp1::blTireTemp0->get()
            ),
            keyValuePair(
                "BL Tire Temp 1",
                dbc::blBrokerTemp1::blTireTemp1->get()
            ),
            keyValuePair(
                "BL Tire Temp 2",
                dbc::blBrokerTemp1::blTireTemp2->get()
            ),
            keyValuePair(
                "BL Tire Temp 3",
                dbc::blBrokerTemp1::blTireTemp3->get()
            ),
            keyValuePair(
                "BL Tire Temp 4",
                dbc::blBrokerTemp2::blTireTemp4->get()
            ),
            keyValuePair(
                "BL Tire Temp 5",
                dbc::blBrokerTemp2::blTireTemp5->get()
            ),
            keyValuePair(
                "BL Tire Temp 6",
                dbc::blBrokerTemp2::blTireTemp6->get()
            ),
            keyValuePair(
                "BL Tire Temp 7",
                dbc::blBrokerTemp2::blTireTemp7->get()
            ),
            keyValuePair(
                "BL Strain Gauge Uncalibrated",
                dbc::blBrokerSusUncalibrated::blStrainGaugeUncalibrated->get()
            ),
            keyValuePair(
                "BL Sus Pot Uncalibrated",
                dbc::blBrokerSusUncalibrated::blSusPotUncalibrated->get()
            ),
            keyValuePair(
                "BL Strain Gauge Error",
                dbc::blBrokerCanErrorMsg::blStrainGaugeError->get()
            ),
            keyValuePair(
                "BL Sus Pot Error",
                dbc::blBrokerCanErrorMsg::blSusPotError->get()
            ),
            keyValuePair(
                "BL Tire Temp Error",
                dbc::blBrokerCanErrorMsg::blTireTempError->get()
            ),
            keyValuePair(
                "BL Heartbeat Count",
                dbc::blBrokerCanErrorMsg::blHeartbeatCount->get()
            ),
            keyValuePair(
                "BL Strain Gauge Calibrated",
                dbc::blBrokerSusCalibrated::blStrainGaugeCalibrated->get()
            ),
            keyValuePair(
                "BL Sus Pot Calibrated",
                dbc::blBrokerSusCalibrated::blSusPotCalibrated->get()
            ),

            ui::vspacer(10),
            ui::h2("Back Right Broker"),
            keyValuePair(
                "BR Tire Temp 0",
                dbc::brBrokerTemp1::brTireTemp0->get()
            ),
            keyValuePair(
                "BR Tire Temp 1",
                dbc::brBrokerTemp1::brTireTemp1->get()
            ),
            keyValuePair(
                "BR Tire Temp 2",
                dbc::brBrokerTemp1::brTireTemp2->get()
            ),
            keyValuePair(
                "Br Tire Temp 3",
                dbc::brBrokerTemp1::brTireTemp3->get()
            ),
            keyValuePair(
                "BR Tire Temp 4",
                dbc::brBrokerTemp2::brTireTemp4->get()
            ),
            keyValuePair(
                "BR Tire Temp 5",
                dbc::brBrokerTemp2::brTireTemp5->get()
            ),
            keyValuePair(
                "BR Tire Temp 6",
                dbc::brBrokerTemp2::brTireTemp6->get()
            ),
            keyValuePair(
                "BR Tire Temp 7",
                dbc::brBrokerTemp2::brTireTemp7->get()
            ),
            keyValuePair(
                "Br Strain Gauge Uncalibrated",
                dbc::brBrokerSusUncalibrated::brStrainGaugeUncalibrated->get()
            ),
            keyValuePair(
                "BR Sus Pot Uncalibrated",
                dbc::brBrokerSusUncalibrated::brSusPotUncalibrated->get()
            ),
            keyValuePair(
                "BR Strain Gauge Error",
                dbc::brBrokerCanErrorMsg::brStrainGaugeError->get()
            ),
            keyValuePair(
                "BR Sus Pot Error",
                dbc::brBrokerCanErrorMsg::brSusPotError->get()
            ),
            keyValuePair(
                "BR Tire Temp Error",
                dbc::brBrokerCanErrorMsg::brTireTempError->get()
            ),
            keyValuePair(
                "BR Heartbeat Count",
                dbc::brBrokerCanErrorMsg::brHeartbeatCount->get()
            ),
            keyValuePair(
                "BR Strain Gauge Calibrated",
                dbc::brBrokerSusCalibrated::brStrainGaugeCalibrated->get()
            ),
            keyValuePair(
                "BR Sus Pot Calibrated",
                dbc::brBrokerSusCalibrated::brSusPotCalibrated->get()
            )
        );
        // clang-format off
    }


    okay::UIElement buildIMU() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
            ui::h2("IMU Status"),
            keyValuePair(
                    "X Axis Acceleration", 
                    dbc::imuAcceleration::xAxisAcceleration->get()
            ),

            keyValuePair(
                    "Y Axis Acceleration", 
                    dbc::imuAcceleration::yAxisAcceleration->get()
            ),

            keyValuePair(
                    "Z Axis Acceleration", 
                    dbc::imuAcceleration::zAxisAcceleration->get()
            ),

            keyValuePair(
                    "Yaw", 
                    dbc::imuYawPitchRoll::yaw->get()
            ),

            keyValuePair(
                    "Pitch", 
                    dbc::imuYawPitchRoll::pitch->get()
            ),

            keyValuePair(
                    "Roll", 
                    dbc::imuYawPitchRoll::roll->get()
            ),

            keyValuePair(
                    "X Axis Angular Rate", 
                    dbc::imuAngularRate::xAxisAngularRate->get()
            ),

            keyValuePair(
                    "Y Axis Angular Rate", 
                    dbc::imuAngularRate::yAxisAngularRate->get()
            ),

            keyValuePair(
                    "Z Axis Angular Rate", 
                    dbc::imuAngularRate::zAxisAngularRate->get()
            ),

            keyValuePair(
                    "Position Latitude", 
                    dbc::imuPositionIns::positionLatitude->get()
            ),

            keyValuePair(
                    "Position Longitude", 
                    dbc::imuPositionIns::positionLongitude->get()
            ),

            keyValuePair(
                    "Position Altutude", 
                    dbc::imuPositionIns::positionAltutude->get()
            ),

            keyValuePair(
                    "X Axis Velocity", 
                    dbc::imuVelocity::xAxisVelocity->get()
            ),

            keyValuePair(
                    "Y Axis Velocity", 
                    dbc::imuVelocity::yAxisVelocity->get()
            ),

            keyValuePair(
                    "Z Axis Velocity", 
                    dbc::imuVelocity::zAxisVelocity->get()
            ),

            keyValuePair(
                    "X Axis Magnetometer", 
                    dbc::imuMag::xAxisMagnetometer->get()
            ),

            keyValuePair(
                    "Y Axis Magnetometer", 
                    dbc::imuMag::yAxisMagnetometer->get()
            ),

            keyValuePair(
                    "Z Axis Magnetometer", 
                    dbc::imuMag::zAxisMagnetometer->get()
            ),

            keyValuePair(
                    "Temperature", 
                    dbc::imuPresTemp::temperature->get()
            ),

            keyValuePair(
                    "Pressure", 
                    dbc::imuPresTemp::pressure->get()
            ),

            keyValuePair(
                    "No G X Axis Acceleration", 
                    dbc::imuAccelerationNoG::noGXAxisAcceleration->get()
            ),

            keyValuePair(
                    "No G Y Axis Acceleration", 
                    dbc::imuAccelerationNoG::noGYAxisAcceleration->get()
            ),

            keyValuePair(
                    "No G Z Axis Acceleration", 
                    dbc::imuAccelerationNoG::noGZAxisAcceleration->get()
            ),

            keyValuePair(
                    "INS Mode", 
                    dbc::imuInsStatus::insMode->get()
            ),

            keyValuePair(
                    "GNSS Fix", 
                    dbc::imuInsStatus::gnssFix->get()
            ),

            keyValuePair(
                    "INS Error", 
                    dbc::imuInsStatus::insError->get()
            ),

            keyValuePair(
                    "GNSS Heading INS", 
                    dbc::imuInsStatus::gnssHeadingIns->get()
            ),

            keyValuePair(
                    "GNSS Compass", 
                    dbc::imuInsStatus::gnssCompass->get()
            ),

            keyValuePair(
                    "UTC Year", 
                    dbc::imuUtcTime::utcYear->get()
            ),

            keyValuePair(
                    "UTC Month", 
                    dbc::imuUtcTime::utcMonth->get()
            ),

            keyValuePair(
                    "UTC Day", 
                    dbc::imuUtcTime::utcDay->get()
            ),

            keyValuePair(
                    "UTC Hour", 
                    dbc::imuUtcTime::utcHour->get()
            ),

            keyValuePair(
                    "UTC Minutes", 
                    dbc::imuUtcTime::utcMinutes->get()
            ),

            keyValuePair(
                    "UTC Seconds", 
                    dbc::imuUtcTime::utcSeconds->get()
            ),

            keyValuePair(
                    "UTC Milliseconds", 
                    dbc::imuUtcTime::utcMilliseconds->get()
            ),

            keyValuePair(
                    "X Axis Delta Velocity", 
                    dbc::imuDeltaVel::xAxisDeltaVelocity->get()
            ),

            keyValuePair(
                    "Y Axis Delta Velocity", 
                    dbc::imuDeltaVel::yAxisDeltaVelocity->get()
            ),

            keyValuePair(
                    "Z Axis Delta Velocity", 
                    dbc::imuDeltaVel::zAxisDeltaVelocity->get()
            )
        );
        // clang-format on
    }

    okay::UIElement buildTelemetry() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
            ui::h2("Telemetry Faults"),
            keyValuePair(
                    "RTC Hour", 
                    dbc::telemetryRtcTime::rtcHour->get()
            ),

            keyValuePair(
                    "RTC Minute", 
                    dbc::telemetryRtcTime::rtcMinute->get()
            ),

            keyValuePair(
                    "RTC Second", 
                    dbc::telemetryRtcTime::rtcSecond->get()
            ),

            keyValuePair(
                    "RTC Subsecond", 
                    dbc::telemetryRtcTime::rtcSubsecond->get()
            ),

            keyValuePair(
                    "RTC Year", 
                    dbc::telemetryRtcDate::rtcYear->get()
            ),

            keyValuePair(
                    "RTC Month", 
                    dbc::telemetryRtcDate::rtcMonth->get()
            ),

            keyValuePair(
                    "RTC Day", 
                    dbc::telemetryRtcDate::rtcDay->get()
            ),

            keyValuePair(
                    "RTC Weekday", 
                    dbc::telemetryRtcDate::rtcWeekday->get()
            ),

            keyValuePair(
                    "Miles Driven", 
                    dbc::telemetryOdometer::milesDriven->get()
            ),

            keyValuePair(
                    "Log File", 
                    dbc::telemetryStatus::logFile->get()
            ),

            ui::vspacer(10),
            ui::h2("PDM"),
            keyValuePair(
                    "Gen Amps", 
                    dbc::pdmCurrent::genAmps->get()
            ),

            keyValuePair(
                    "Front Fan Amps", 
                    dbc::pdmCurrent::frontFanAmps->get()
            ),

            keyValuePair(
                    "Rear Fan Amps", 
                    dbc::pdmCurrent::rearFanAmps->get()
            ),

            keyValuePair(
                    "Front Pump Amps", 
                    dbc::pdmCurrent::frontPumpAmps->get()
            ),

            keyValuePair(
                    "Rear Pump Amps", 
                    dbc::pdmCurrent::rearPumpAmps->get()
            ),

            keyValuePair(
                    "Bat Volt", 
                    dbc::pdmBatVolt::batVolt->get()
            ),

            keyValuePair(
                    "Low Bat Volt Warning", 
                    dbc::pdmBatVolt::lowBatVoltWarning->get()
            ),

            keyValuePair(
                    "Danger Bat Volt Warning", 
                    dbc::pdmBatVolt::dangerBatVoltWarning->get()
            ),

            keyValuePair(
                    "Coolant 1 Volt", 
                    dbc::pdmCoolant::coolant1Volt->get()
            ),

            keyValuePair(
                    "Coolant 2 Volt", 
                    dbc::pdmCoolant::coolant2Volt->get()
            ),

            keyValuePair(
                    "Coolant 3 Volt", 
                    dbc::pdmCoolant::coolant3Volt->get()
            )
        );
        // clang-format on
    }

    template <typename T>
    inline okay::UIElement keyValuePair(const std::string& key, const T& value) {
        // clang-format off
        return ui::slot(okay::UIAxis::Horizontal)
            .widthGrow() (
                ui::h3(key),
                ui::spacer(),
                ui::h3(std::format("{}", value))
            );
        // clang-format on
    }

    inline okay::UIElement keyErrorValuePair(const std::string& key, bool value) {
        if (value == true) {
            // clang-format off
            return ui::slot(okay::UIAxis::Horizontal)
                .widthGrow() (
                    ui::h3(key)
                        .textColorSet(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
                    ui::spacer(),
                    ui::h3(std::format("{}", value))
                        .textColorSet(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))
                );
            // clang-format on
        }

        return keyValuePair(key, value);
    }

    void closePage() {
        for (okay::ECSEntity& entity : _entities) {
            entity.destroy();
        }
    }

   private:
    std::vector<okay::ECSEntity> _entities;

};

}  // namespace dash

#endif

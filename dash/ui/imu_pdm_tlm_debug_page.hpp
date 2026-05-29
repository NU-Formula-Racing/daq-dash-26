#ifndef __IMU_PDM_TLM_DEBUG_PAGE_H__
#define __IMU_PDM_TLM_DEBUG_PAGE_H__

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

class IMUPDMTLMDebugPage : public IPage {
   public:
    IMUPDMTLMDebugPage() {}

    void initializePage() {
        okay::Engine.logger.debug("Creating entities for IMU/TELEM/PDM!");

        okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
            SharedElements::get().skyboxMaterial);
        okay::UIStyle::main().setMainFont(*fonts::latoBold);

        _entities = {
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 1),
            okay::ecs::uiEntity(BIND_TO_THIS(buildIMUPDMTLMDebug), 3),
        };
    }

    okay::UIElement buildIMUPDMTLMDebug() {
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
                            ui::h1("IMU (cont'd)"), // both back brokers
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

    okay::UIElement buildIMU1() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
        ui::h2("IMU"),  
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
            )
        );
        // clang-format off
    }

     okay::UIElement buildIMU2() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
        ui::h2("IMU (cont'd)"),
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
        // clang-format off
    } 

    okay::UIElement buildTelemetry() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
        ui::h2("Telemetry"),
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
            )
        );
        // clang-format off
    } 

    okay::UIElement buildPDM() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
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
        // clang-format off
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

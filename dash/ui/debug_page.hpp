#ifndef __DEBUG_PAGE_H__
#define __DEBUG_PAGE_H__
#include "okay/core/ui/builder.hpp"
#include "okay/core/ui/element.hpp"
#include "page.hpp"
#include "shared_elements.hpp"
#include "style.hpp"

#include <okay/okay.hpp>

#include <can/can_dbc.hpp>
#include <memory>

namespace ui = okay::ui;

namespace dash {

#ifndef BIND_TO_THIS
#define BIND_TO_THIS(fnName)   \
    [this]() {                 \
        return this->fnName(); \
    }
#endif

class DebugPage : public IPage {
   public:
    DebugPage() {}

    void initializePage() {
        okay::Engine.logger.debug("Creating entities for Error page!");

        okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
            SharedElements::get().skyboxMaterial);
        okay::UIStyle::main().setMainFont(*fonts::latoBold);

        _entities = {
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 1),
            okay::ecs::uiEntity(BIND_TO_THIS(buildDebug), 3),
        };
    }

    okay::UIElement buildDebug() {
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
                            ui::h1("VCU"),
                            buildVCU()
                        ),
                        buildContainer()(
                            ui::h1("BMS"),
                            buildBMS()
                        ),
                        buildContainer()(
                            ui::h1("INVERTER"),
                            buildInverter()
                        ),
                        buildContainer()(
                            ui::h1("FAULTS"),
                            buildFaults()
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

    okay::UIElement buildVCU() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
            ui::h2("Drive/Driver Interface"),
            keyValuePair(
                "Drive Status",
                dbc::ecuDriveStatus::driveState->get()
            ),
            keyValuePair(
                "APPS1",
                dbc::ecuThrottle::apps1Throttle->get()
            ),
            keyValuePair(
                "APPS2",
                dbc::ecuThrottle::apps2Throttle->get()
            ),
            keyValuePair(
                "Font Brake Pressure",
                dbc::ecuBrake::frontBrakePressure->get()
            ),
            keyValuePair(
                "Rear Brake Pressure",
                dbc::ecuBrake::rearBrakePressure->get()
            ),
            keyValuePair(
                "Brake Pressed",
                dbc::ecuBrake::brakePressed->get()
            ),

            ui::vspacer(10),
            ui::h2("Commands"),
            keyValuePair(
                "Set Current (Rear)",
                dbc::ecuSetCurrentRearInverter::setCurrentRearInverter->get()
            ),
            keyValuePair(
                "Set Current (Left)",
                dbc::ecuSetCurrentFrontLeftInverter::setCurrentFrontLeftInverter->get()
            ),
            keyValuePair(
                "Set Current (Right)",
                dbc::ecuSetCurrentBrakeFrontRightInverter::setCurrentBrakeFrontRightInverter->get()
            ),
            keyValuePair(
                "BMS Command",
                dbc::ecuBmsCommandMessage::bmsCommand->get()
            )
        );
        // clang-format off
    }

    okay::UIElement buildBMS() {
            // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
            ui::h2("SOE"),
            keyValuePair(
                "Max Discharge Current",
                dbc::bmsSoe::maxDischargeCurrent->get()
            ),
            keyValuePair(
                "Max Regen Current",
                dbc::bmsSoe::maxRegenCurrent->get()
            ),
            keyValuePair(
                "Battery Voltage",
                dbc::bmsSoe::batteryVoltage->get()
            ),
            keyValuePair(
                "Battery Temperature",
                dbc::bmsSoe::batteryTemperature->get()
            ),
            keyValuePair(
                "Battery Current",
                dbc::bmsSoe::batteryCurrent->get()
            ),

            ui::vspacer(10),
            ui::h2("Status"),
            keyValuePair(
                "BMS State",
                dbc::bmsStatus::bmsState->get()
            ),
            keyValuePair(
                "IMD State",
                dbc::bmsStatus::imdState->get()
            ),
            keyValuePair(
                "Max Cell Temp",
                dbc::bmsStatus::maxCellTemp->get()
            ),
            keyValuePair(
                "Min Cell Temp",
                dbc::bmsStatus::minCellTemp->get()
            ),
            keyValuePair(
                "Max Cell Voltage",
                dbc::bmsStatus::maxCellVoltage->get()
            ),
            keyValuePair(
                "Min Cell Voltage",
                dbc::bmsStatus::minCellVoltage->get()
            ),
            keyValuePair(
                "BMS SOC",
                dbc::bmsStatus::bmsSoc->get()
            )
        );
        // clang-format on
    }

    okay::UIElement buildInverter() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
            ui::h2("Rear Motor Status"),
            keyValuePair(
                "RPM",
                dbc::rearInverterMotorStatus::rpm->get()
            ),
            keyValuePair(
                "Motor Current",
                dbc::rearInverterMotorStatus::motorCurrent->get()
            ),
            keyValuePair(
                "DC Voltage",
                dbc::rearInverterMotorStatus::dcVoltage->get()
            ),
            keyValuePair(
                "DC Current",
                dbc::rearInverterMotorStatus::dcCurrent->get()
            ),

            ui::vspacer(10),
            ui::h2("Rear Temperature"),
            keyValuePair(
                "IGBT Temp",
                dbc::rearInverterTempStatus::igbtTemp->get()
            ),
            keyValuePair(
                "Motor Temp",
                dbc::rearInverterTempStatus::motorTemp->get()
            ),

            ui::vspacer(10),
            ui::h2("Rear Power"),
            keyValuePair(
                "Ah Drawn",
                dbc::rearInverterCurrentDraw::ahDrawn->get()
            ),
            keyValuePair(
                "Ah Charged",
                dbc::rearInverterCurrentDraw::ahCharged->get()
            ),
            keyValuePair(
                "Wh Drawn",
                dbc::rearInverterPowerDraw::whDrawn->get()
            ),
            keyValuePair(
                "Wh Charged",
                dbc::rearInverterPowerDraw::whCharged->get()
            ),

            ui::vspacer(10),
            ui::h2("Rear Inverter"),
            keyValuePair(
                "Fault Code",
                dbc::rearInverterFaultStatus::faultCode->get()
            )
        );
        // clang-format on
    }

    okay::UIElement buildFaults() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
            ui::h2("VCU Implausibilities"),
            keyErrorValuePair(
                "Implausibility Present",
                dbc::ecuImplausibility::implausibilityPresent->get()
            ),
            keyErrorValuePair(
                "APPS Disagreement",
                dbc::ecuImplausibility::appssDisagreementImp->get()
            ),
            keyErrorValuePair(
                "BPPC Implausibility",
                dbc::ecuImplausibility::bppcImp->get()
            ),
            keyErrorValuePair(
                "Brake Invalid",
                dbc::ecuImplausibility::brakeInvalidImp->get()
            ),
            keyErrorValuePair(
                "APPS Invalid",
                dbc::ecuImplausibility::appssInvalidImp->get()
            ),

            ui::vspacer(10),
            ui::h2("BMS Faults"),
            keyErrorValuePair(
                "Internal Fault Summary",
                dbc::bmsFaults::internalfaultSummary->get()
            ),
            keyErrorValuePair(
                "External Fault",
                dbc::bmsFaults::externalFault->get()
            ),
            keyErrorValuePair(
                "Undervoltage Fault",
                dbc::bmsFaults::undervoltageFault->get()
            ),
            keyErrorValuePair(
                "Overvoltage Fault",
                dbc::bmsFaults::overvoltageFault->get()
            ),
            keyErrorValuePair(
                "Undertemperature Fault",
                dbc::bmsFaults::undertemperatureFault->get()
            ),
            keyErrorValuePair(
                "Overtemperature Fault",
                dbc::bmsFaults::overtemperatureFault->get()
            ),
            keyErrorValuePair(
                "Overcurrent Fault",
                dbc::bmsFaults::overcurrentFault->get()
            ),
            keyErrorValuePair(
                "Open Wire Fault",
                dbc::bmsFaults::openWireFault->get()
            ),
            keyErrorValuePair(
                "Open Wire Temp Fault",
                dbc::bmsFaults::openWireTempFault->get()
            ),
            keyErrorValuePair(
                "PEC Fault",
                dbc::bmsFaults::pecFault->get()
            ),
            keyValuePair(
                "Total PEC Failures",
                dbc::bmsFaults::totalPecFailures->get()
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

    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> bgTexture{
        "textures/bg_pattern.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> tempFull{"textures/temp_full.png"};
};

}  // namespace dash

#endif

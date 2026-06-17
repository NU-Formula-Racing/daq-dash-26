#include "debug_page.hpp"

#include "okay/core/ui/builder.hpp"
#include "okay/core/ui/element.hpp"
#include "page.hpp"
#include "shared_elements.hpp"
#include "style.hpp"

#include <can/can_dbc.hpp>
#include <memory>

namespace ui = okay::ui;

namespace dash {

void DebugPage::initializePage() {
    okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
        SharedElements::get().skyboxMaterial);
    okay::UIStyle::main().setMainFont(*fonts::latoBold);

    _entities = {
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 3),
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 3),
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 2),
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildLaunchControlIndicator), 1),
        okay::ecs::uiEntity(BIND_TO_THIS(buildDebug), 3),
    };
}

void DebugPage::closePage() {
    for (okay::ECSEntity& entity : _entities) {
        entity.destroy();
    }
}

okay::UIElement DebugPage::buildDebug() {
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

okay::UIElement DebugPage::buildContainer() {
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

okay::UIElement DebugPage::buildVCU() {
    // clang-format off
    return ui::growbox(okay::UIAxis::Vertical) (
        ui::h2("Drive/Driver Interface"),
        keyValuePair(
            "Drive Status",
            dbc::vcuDriveStatus::driveState->get()
        ),
        keyValuePair(
            "APPS1",
            dbc::vcuThrottle::apps1Throttle->get()
        ),
        keyValuePair(
            "APPS1 Raw",
            dbc::vcuThrottle::apps1ThrottleRaw->get()
        ),
        keyValuePair(
            "APPS2",
            dbc::vcuThrottle::apps2Throttle->get()
        ),
        keyValuePair(
            "APPS2 Raw",
            dbc::vcuThrottle::apps2ThrottleRaw->get()
        ),
        keyValuePair(
            "Font Brake Pressure",
            dbc::vcuBrake::frontBrakePressure->get()
        ),
        keyValuePair(
            "Rear Brake Pressure",
            dbc::vcuBrake::rearBrakePressure->get()
        ),
        // keyValuePair(
        //     "Brake Pressed",
        //     dbc::vcuBrake::brakePressed->get()
        // ),

        ui::vspacer(5),
        ui::h2("Commands"),
        keyValuePair(
            "Set Current (Rear)",
            dbc::vcuSetCurrentRearInverter::setCurrentRearInverter->get()
        ),
        keyValuePair(
            "Set Current (Left)",
            dbc::vcuSetCurrentFrontLeftInverter::setCurrentFrontLeftInverter->get()
        ),
        keyValuePair(
            "Set Current (Right)",
            dbc::vcuSetCurrentBrakeFrontRightInverter::setCurrentBrakeFrontRightInverter->get()
        ),
        keyValuePair(
            "BMS Command",
            dbc::vcuBmsCommandMessage::bmsCommand->get()
        ),

        ui::vspacer(5),
        ui::h2("VCU Implausibilities"),
        keyErrorValuePair(
            "Implausibility Present",
            dbc::vcuImplausibility::implausibilityPresent->get()
        ),
        keyErrorValuePair(
            "APPS Disagreement",
            dbc::vcuImplausibility::appssDisagreementImp->get()
        ),
        keyErrorValuePair(
            "BPPC Implausibility",
            dbc::vcuImplausibility::bppcImp->get()
        ),
        keyErrorValuePair(
            "Brake Invalid",
            dbc::vcuImplausibility::brakeInvalidImp->get()
        ),
        keyErrorValuePair(
            "APPS Invalid",
            dbc::vcuImplausibility::appssInvalidImp->get()
        )
    );
    // clang-format off
}

okay::UIElement DebugPage::buildBMS() {
    // clang-format off
    return ui::growbox(okay::UIAxis::Vertical) (
        ui::h2("Packboard"),
        keyValuePair(
            "Battery Current",
            dbc::bmsPackboard::batteryCurrent->get()
        ),
        keyValuePair(
            "Packboard Voltage",
            dbc::bmsPackboard::packboardVoltage->get()
        ),

        ui::vspacer(10),
        ui::h2("Daughterboard"),
        keyValuePair(
            "Battery Voltage",
            dbc::bmsDaughterboard::batteryVoltage->get()
        ),
        keyValuePair(
            "Max Cell Voltage",
            dbc::bmsDaughterboard::maxCellVoltage->get()
        ),
        keyValuePair(
            "Min Cell Voltage",
            dbc::bmsDaughterboard::minCellVoltage->get()
        ),
        keyValuePair(
            "Battery Temperature",
            dbc::bmsDaughterboard::batteryTemperature->get()
        ),

        ui::vspacer(10),
        ui::h2("Status"),
        keyValuePair(
            "SOC",
            dbc::bmsStatus::soc->get()
        ),
        keyValuePair(
            "BMS State",
            dbc::bmsStatus::bmsState->get()
        ),
        keyValuePair(
            "IMD State",
            dbc::bmsStatus::imdState->get()
        ),
        keyValuePair(
            "Total PEC Failures",
            dbc::bmsStatus::totalPecFailures->get()
        ),

        ui::vspacer(10),
        ui::h2("Timeouts"),
        keyErrorValuePair(
            "VCU Timeout",
            dbc::bmsStatus::vcuTimeout->get()
        ),
        keyErrorValuePair(
            "Inverter Timeout",
            dbc::bmsStatus::inverterTimeout->get()
        ),
        keyErrorValuePair(
            "Charger Timeout",
            dbc::bmsStatus::chargerTimeout->get()
        )
    );
    // clang-format on
}

okay::UIElement DebugPage::buildInverter() {
    // clang-format off
    return ui::growbox(okay::UIAxis::Vertical) (
        ui::h2("Rear Motor Status"),
        keyValuePair(
            "RPM",
            dbc::rearInverterMotorStatus::bRpm->get()
        ),
        keyValuePair(
            "Motor Current",
            dbc::rearInverterMotorStatus::bMotorCurrent->get()
        ),
        keyValuePair(
            "DC Voltage",
            dbc::rearInverterMotorStatus::bDcVoltage->get()
        ),
        keyValuePair(
            "DC Current",
            dbc::rearInverterMotorStatus::bDcCurrent->get()
        ),

        ui::vspacer(10),
        ui::h2("Rear Temperature"),
        keyValuePair(
            "IGBT Temp",
            dbc::rearInverterTempStatus::bIgbtTemp->get()
        ),
        keyValuePair(
            "Motor Temp",
            dbc::rearInverterTempStatus::bMotorTemp->get()
        ),

        ui::vspacer(10),
        ui::h2("Rear Power"),
        keyValuePair(
            "Ah Drawn",
            dbc::rearInverterCurrentDraw::bAhDrawn->get()
        ),
        keyValuePair(
            "Ah Charged",
            dbc::rearInverterCurrentDraw::bAhCharged->get()
        ),
        keyValuePair(
            "Wh Drawn",
            dbc::rearInverterPowerDraw::bWhDrawn->get()
        ),
        keyValuePair(
            "Wh Charged",
            dbc::rearInverterPowerDraw::bWhCharged->get()
        ),

        ui::vspacer(10),
        ui::h2("Rear Inverter"),
        keyValuePair(
            "Fault Code",
            dbc::rearInverterFaultStatus::bFaultCode->get()
        )
    );
    // clang-format on
}

okay::UIElement DebugPage::buildFaults() {
    // clang-format off
    return ui::growbox(okay::UIAxis::Vertical) (
        ui::h2("BMS Faults"),
        keyErrorValuePair(
            "Internal Fault Summary",
            dbc::bmsStatus::internalfaultSummary->get()
        ),
        keyErrorValuePair(
            "Undervoltage Fault",
            dbc::bmsStatus::undervoltageFault->get()
        ),
        keyErrorValuePair(
            "Overvoltage Fault",
            dbc::bmsStatus::overvoltageFault->get()
        ),
        keyErrorValuePair(
            "Undertemperature Fault",
            dbc::bmsStatus::undertemperatureFault->get()
        ),
        keyErrorValuePair(
            "Overtemperature Fault",
            dbc::bmsStatus::overtemperatureFault->get()
        ),
        keyErrorValuePair(
            "Open Wire Fault",
            dbc::bmsStatus::openWireFault->get()
        ),
        keyErrorValuePair(
            "Open Wire Temp Fault",
            dbc::bmsStatus::openWireTempFault->get()
        ),
        keyErrorValuePair(
            "PEC Fault",
            dbc::bmsStatus::pecFault->get()
        ),
        keyErrorValuePair(
            "Shutdown Open",
            dbc::bmsStatus::shutdownOpen->get()
        ),

        ui::vspacer(10),
        ui::h2("BSPD"),
        keyErrorValuePair(
            "High Current",
            dbc::vcuBspdStatus::highCurrent->get()
        ),
        keyErrorValuePair(
            "Hard Brake",
            dbc::vcuBspdStatus::hardBrake->get()
        ),
        keyErrorValuePair(
            "Brake Error",
            dbc::vcuBspdStatus::brakeError->get()
        ),
        keyErrorValuePair(
            "Current Error",
            dbc::vcuBspdStatus::currentError->get()
        ),
        keyErrorValuePair(
            "BSPD Error",
            dbc::vcuBspdStatus::bspdErrorB->get()
        ),
        keyErrorValuePair(
            "BSPD Sensor Error",
            dbc::vcuBspdStatus::bspdSensErrorB->get()
        )
    );
    // clang-format on
}

}  // namespace dash

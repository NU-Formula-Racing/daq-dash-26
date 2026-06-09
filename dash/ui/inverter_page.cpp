#include "inverter_page.hpp"

#include "can/can_dbc.hpp"
#include "page.hpp"
#include "shared_elements.hpp"
#include "style.hpp"

#include <vector>

namespace ui = okay::ui;

namespace dash {

void InverterPage::initializePage() {
    okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
        SharedElements::get().skyboxMaterial);
    okay::UIStyle::main().setMainFont(*fonts::latoBold);

    _entities = {okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 3),
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 3),
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 2),
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildLaunchControlIndicator), 1),
        okay::ecs::uiEntity(BIND_TO_THIS(buildDebug), 3)};
}

void InverterPage::closePage() {
    for (okay::ECSEntity& entity : _entities) {
        entity.destroy();
    }
}

okay::UIElement InverterPage::buildDebug() {
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
                        ui::h1("FL INVERTER"),
                        buildFrontLeftInverter()
                    ),
                    buildContainer()(
                        ui::h1("REAR INVERTER"),
                        buildRearInverter()
                    ),
                    buildContainer()(
                        ui::h1("FR INVERTER"),
                        buildFrontRightInverter()
                    ),
                    buildContainer()(
                        ui::h1("VCU CMDS"),
                        buildVCUInverter()
                    ),
                    ui::spacer()
                ),
            ui::spacer()
        );
    // clang-format on
}

okay::UIElement InverterPage::buildContainer() {
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

okay::UIElement InverterPage::buildFrontRightInverter() {
    return ui::growbox(okay::UIAxis::Vertical)(ui::h2("Status"),
        keyValuePair("RPM", dbc::rearInverterMotorStatus::bRpm->get()),
        keyValuePair("Motor Current", dbc::frontRightInverterMotorStatus::frMotorCurrent->get()),
        keyValuePair("DC Voltage", dbc::frontRightInverterMotorStatus::frDcVoltage->get()),
        keyValuePair("DC Current", dbc::frontRightInverterMotorStatus::frDcCurrent->get()),

        ui::vspacer(10),
        ui::h2("Temperature"),
        keyValuePair("IGBT Temp", dbc::frontRightInverterTempStatus::frIgbtTemp->get()),
        keyValuePair("Motor Temp", dbc::frontRightInverterTempStatus::frMotorTemp->get()),

        ui::vspacer(10),
        ui::h2("Power"),
        keyValuePair("Ah Drawn", dbc::frontRightInverterCurrentDraw::frAhDrawn->get()),
        keyValuePair("Ah Charged", dbc::frontRightInverterCurrentDraw::frAhCharged->get()),
        keyValuePair("Wh Drawn", dbc::frontRightInverterPowerDraw::frWhDrawn->get()),
        keyValuePair("Wh Charged", dbc::frontRightInverterPowerDraw::frWhCharged->get()),
        keyValuePair("Fault Code", dbc::frontRightInverterFaultStatus::frFaultCode->get()));
}

okay::UIElement InverterPage::buildFrontLeftInverter() {
    return ui::growbox(okay::UIAxis::Vertical)(ui::h2("Status"),
        keyValuePair("Motor Current", dbc::frontLeftInverterMotorStatus::flMotorCurrent->get()),
        keyValuePair("DC Voltage", dbc::frontLeftInverterMotorStatus::flDcVoltage->get()),
        keyValuePair("DC Current", dbc::frontLeftInverterMotorStatus::flDcCurrent->get()),

        ui::vspacer(10),
        ui::h2("Temperature"),
        keyValuePair("IGBT Temp", dbc::frontLeftInverterTempStatus::flIgbtTemp->get()),
        keyValuePair("Motor Temp", dbc::frontLeftInverterTempStatus::flMotorTemp->get()),

        ui::vspacer(10),
        ui::h2("Power"),
        keyValuePair("Ah Drawn", dbc::frontLeftInverterCurrentDraw::flAhDrawn->get()),
        keyValuePair("Ah Charged", dbc::frontLeftInverterCurrentDraw::flAhCharged->get()),
        keyValuePair("Wh Drawn", dbc::frontLeftInverterPowerDraw::flWhDrawn->get()),
        keyValuePair("Wh Charged", dbc::frontLeftInverterPowerDraw::flWhCharged->get()),
        keyValuePair("Fault Code", dbc::frontLeftInverterFaultStatus::flFaultCode->get()));
}

okay::UIElement InverterPage::buildRearInverter() {
    return ui::growbox(okay::UIAxis::Vertical)(ui::h2("Status"),
        keyValuePair("Motor Current", dbc::rearInverterMotorStatus::bMotorCurrent->get()),
        keyValuePair("DC Voltage", dbc::rearInverterMotorStatus::bDcVoltage->get()),
        keyValuePair("DC Current", dbc::rearInverterMotorStatus::bDcCurrent->get()),

        ui::vspacer(10),
        ui::h2("Temperature"),
        keyValuePair("IGBT Temp", dbc::rearInverterTempStatus::bIgbtTemp->get()),
        keyValuePair("Motor Temp", dbc::rearInverterTempStatus::bMotorTemp->get()),

        ui::vspacer(10),
        ui::h2("Power"),
        keyValuePair("Ah Drawn", dbc::rearInverterCurrentDraw::bAhDrawn->get()),
        keyValuePair("Ah Charged", dbc::rearInverterCurrentDraw::bAhCharged->get()),
        keyValuePair("Wh Drawn", dbc::rearInverterPowerDraw::bWhDrawn->get()),
        keyValuePair("Wh Charged", dbc::rearInverterPowerDraw::bWhCharged->get()),
        keyValuePair("Fault Code", dbc::rearInverterFaultStatus::bFaultCode->get()));
}

okay::UIElement InverterPage::buildVCUInverter() {
    return ui::growbox(okay::UIAxis::Vertical)(ui::h2("Front Left Current"),
        keyValuePair(
            "Set Current", dbc::vcuSetCurrentFrontLeftInverter::setCurrentFrontLeftInverter->get()),
        keyValuePair("Set Current Brake",
            dbc::vcuSetCurrentBrakeFrontLeftInverter::setCurrentBrakeFrontLeftInverter->get()),

        ui::vspacer(10),
        ui::h2("Front Right Current"),
        keyValuePair("Set Current",
            dbc::vcuSetCurrentFrontRightInverter::setCurrentFrontRightInverter->get()),
        keyValuePair("Set Current Brake",
            dbc::vcuSetCurrentBrakeFrontRightInverter::setCurrentBrakeFrontRightInverter->get()),

        ui::vspacer(10),
        ui::h2("Rear Current"),
        keyValuePair("Set Current", dbc::vcuSetCurrentRearInverter::setCurrentRearInverter->get()),
        keyValuePair("Set Current Brake",
            dbc::vcuSetCurrentBrakeRearInverter::setCurrentBrakeRearInverter->get()));
}

}  // namespace dash

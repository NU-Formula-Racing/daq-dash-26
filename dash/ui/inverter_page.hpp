#ifndef __INVERTER_PAGE_H__
#define __INVERTER_PAGE_H__

#include "page.hpp"
#include "shared_elements.hpp"
#include "style.hpp"

#include <okay/okay.hpp>

#include <vector>

namespace ui = okay::ui;

namespace dash {

class InverterPage : public dash::IPage {
   public:
    InverterPage() {}

    void initializePage() {
        okay::Engine.logger.debug("Creating entities for Inverter page!");

        okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
            SharedElements::get().skyboxMaterial);
        okay::UIStyle::main().setMainFont(*fonts::latoBold);

        _entities = {okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 1),
            okay::ecs::uiEntity(BIND_TO_THIS(buildDebug), 3)};
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

    okay::UIElement buildFrontRightInverter() {
        return ui::growbox(okay::UIAxis::Vertical)(ui::h2("Status"),
            keyValuePair("RPM", dbc::rearInverterMotorStatus::rpm->get()),
            keyValuePair("Motor Current", dbc::frontRightInverterMotorStatus::motorCurrent->get()),
            keyValuePair("DC Voltage", dbc::frontRightInverterMotorStatus::dcVoltage->get()),
            keyValuePair("DC Current", dbc::frontRightInverterMotorStatus::dcCurrent->get()),

            ui::vspacer(10),
            ui::h2("Temperature"),
            keyValuePair("IGBT Temp", dbc::frontRightInverterTempStatus::igbtTemp->get()),
            keyValuePair("Motor Temp", dbc::frontRightInverterTempStatus::motorTemp->get()),

            ui::vspacer(10),
            ui::h2("Power"),
            keyValuePair("Ah Drawn", dbc::frontRightInverterCurrentDraw::ahDrawn->get()),
            keyValuePair("Ah Charged", dbc::frontRightInverterCurrentDraw::ahCharged->get()),
            keyValuePair("Wh Drawn", dbc::frontRightInverterPowerDraw::whDrawn->get()),
            keyValuePair("Wh Charged", dbc::frontRightInverterPowerDraw::whCharged->get()),
            keyValuePair("Fault Code", dbc::frontRightInverterFaultStatus::faultCode->get()));
    }

    okay::UIElement buildFrontLeftInverter() {
        return ui::growbox(okay::UIAxis::Vertical)(ui::h2("Status"),
            keyValuePair("Motor Current", dbc::frontLeftInverterMotorStatus::motorCurrent->get()),
            keyValuePair("DC Voltage", dbc::frontLeftInverterMotorStatus::dcVoltage->get()),
            keyValuePair("DC Current", dbc::frontLeftInverterMotorStatus::dcCurrent->get()),

            ui::vspacer(10),
            ui::h2("Temperature"),
            keyValuePair("IGBT Temp", dbc::frontLeftInverterTempStatus::igbtTemp->get()),
            keyValuePair("Motor Temp", dbc::frontLeftInverterTempStatus::motorTemp->get()),

            ui::vspacer(10),
            ui::h2("Power"),
            keyValuePair("Ah Drawn", dbc::frontLeftInverterCurrentDraw::ahDrawn->get()),
            keyValuePair("Ah Charged", dbc::frontLeftInverterCurrentDraw::ahCharged->get()),
            keyValuePair("Wh Drawn", dbc::frontLeftInverterPowerDraw::whDrawn->get()),
            keyValuePair("Wh Charged", dbc::frontLeftInverterPowerDraw::whCharged->get()),
            keyValuePair("Fault Code", dbc::frontLeftInverterFaultStatus::faultCode->get()));
    }

    okay::UIElement buildRearInverter() {
        return ui::growbox(okay::UIAxis::Vertical)(ui::h2("Status"),
            keyValuePair("Motor Current", dbc::rearInverterMotorStatus::motorCurrent->get()),
            keyValuePair("DC Voltage", dbc::rearInverterMotorStatus::dcVoltage->get()),
            keyValuePair("DC Current", dbc::rearInverterMotorStatus::dcCurrent->get()),

            ui::vspacer(10),
            ui::h2("Temperature"),
            keyValuePair("IGBT Temp", dbc::rearInverterTempStatus::igbtTemp->get()),
            keyValuePair("Motor Temp", dbc::rearInverterTempStatus::motorTemp->get()),

            ui::vspacer(10),
            ui::h2("Power"),
            keyValuePair("Ah Drawn", dbc::rearInverterCurrentDraw::ahDrawn->get()),
            keyValuePair("Ah Charged", dbc::rearInverterCurrentDraw::ahCharged->get()),
            keyValuePair("Wh Drawn", dbc::rearInverterPowerDraw::whDrawn->get()),
            keyValuePair("Wh Charged", dbc::rearInverterPowerDraw::whCharged->get()),
            keyValuePair("Fault Code", dbc::rearInverterFaultStatus::faultCode->get()));
    }

    okay::UIElement buildVCUInverter() {
        return ui::growbox(okay::UIAxis::Vertical)(ui::h2("Front Left Current"),
            keyValuePair("Set Current Front Left Inverter",
                dbc::ecuSetCurrentFrontLeftInverter::setCurrentFrontLeftInverter->get()),
            keyValuePair("Set Current Brake Front Left Inverter",
                dbc::ecuSetCurrentBrakeFrontLeftInverter::setCurrentBrakeFrontLeftInverter->get()),

            ui::vspacer(10),
            ui::h2("Front Right Current"),
            keyValuePair("Set Current Front Right Inverter",
                dbc::ecuSetCurrentFrontRightInverter::setCurrentFrontRightInverter->get()),
            keyValuePair("Set Current Brake Front Right Inverter",
                dbc::ecuSetCurrentBrakeFrontRightInverter::setCurrentBrakeFrontRightInverter
                    ->get()),

            ui::vspacer(10),
            ui::h2("Rear Current"),
            keyValuePair("Set Current Rear Inverter",
                dbc::ecuSetCurrentRearInverter::setCurrentRearInverter->get()),
            keyValuePair("Set Current Brake Rear Inverter",
                dbc::ecuSetCurrentBrakeRearInverter::setCurrentBrakeRearInverter->get()));
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

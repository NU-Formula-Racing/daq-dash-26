#ifndef __BROKERS_DEBUG_PAGE_H__
#define __BROKERS_DEBUG_PAGE_H__

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

class BrokersDebugPage : public IPage {
   public:
    BrokersDebugPage() {}

    void initializePage() {
        okay::Engine.logger.debug("Creating entities for Brokers Debug page!");

        okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
            SharedElements::get().skyboxMaterial);
        okay::UIStyle::main().setMainFont(*fonts::latoBold);

        _entities = {
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 3),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 3),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildLaunchControlIndicator), 1),
            okay::ecs::uiEntity(BIND_TO_THIS(buildBrokersDebug), 3),
        };
    }

    okay::UIElement buildBrokersDebug() {
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
                            ui::h1("FR BROKER"), // both front brokers
                            buildFrontRightBroker()
                        ),
                        buildContainer()(
                            ui::h1("FL BROKER"), // both back brokers
                            buildFrontLeftBroker()
                        ),
                        buildContainer()(
                            ui::h1("BR BROKER"),
                            buildBackLeftBroker()
                        ),
                        buildContainer()(
                            ui::h1("BL BROKER"),
                            buildBackRightBroker()
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

    okay::UIElement buildFrontLeftBroker() {
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
            )
        );
        // clang-format off
    }

     okay::UIElement buildFrontRightBroker() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
        ui::h2("Front Right Broker"),
            keyValuePair(
                "Tire Temp 0",
                dbc::frBrokerTemp1::frTireTemp0->get()
            ),
            keyValuePair(
                "Tire Temp 1",
                dbc::frBrokerTemp1::frTireTemp1->get()
            ),
            keyValuePair(
                "Tire Temp 2",
                dbc::frBrokerTemp1::frTireTemp2->get()
            ),
            keyValuePair(
                "Tire Temp 3",
                dbc::frBrokerTemp1::frTireTemp3->get()
            ),
            keyValuePair(
                "Tire Temp 4",
                dbc::frBrokerTemp2::frTireTemp4->get()
            ),
            keyValuePair(
                "Tire Temp 5",
                dbc::frBrokerTemp2::frTireTemp5->get()
            ),
            keyValuePair(
                "Tire Temp 6",
                dbc::frBrokerTemp2::frTireTemp6->get()
            ),
            keyValuePair(
                "Tire Temp 7",
                dbc::frBrokerTemp2::frTireTemp7->get()
            ),
            keyValuePair(
                "Str Gauge Uncalibrated",
                dbc::frBrokerSusUncalibrated::frStrainGaugeUncalibrated->get()
            ),
            keyValuePair(
                "Sus Pot Uncalibrated",
                dbc::frBrokerSusUncalibrated::frSusPotUncalibrated->get()
            ),
            keyValuePair(
                "Str Gauge Error",
                dbc::frBrokerCanErrorMsg::frStrainGaugeError->get()
            ),
            keyValuePair(
                "Sus Pot Error",
                dbc::frBrokerCanErrorMsg::frSusPotError->get()
            ),
            keyValuePair(
                "Tire Temp Error",
                dbc::frBrokerCanErrorMsg::frTireTempError->get()
            ),
            keyValuePair(
                "Heartbeat Count",
                dbc::frBrokerCanErrorMsg::frHeartbeatCount->get()
            ),
            keyValuePair(
                "Str Gauge Calibrated",
                dbc::frBrokerSusCalibrated::frStrainGaugeCalibrated->get()
            ),
            keyValuePair(
                "Sus Pot Calibrated",
                dbc::frBrokerSusCalibrated::frSusPotCalibrated->get()
            )
        );
        // clang-format off
    }


    okay::UIElement buildBackLeftBroker() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
        ui::h2("Back Left Broker"),
            keyValuePair(
                "Tire Temp 0",
                dbc::blBrokerTemp1::blTireTemp0->get()
            ),
            keyValuePair(
                "Tire Temp 1",
                dbc::blBrokerTemp1::blTireTemp1->get()
            ),
            keyValuePair(
                "Tire Temp 2",
                dbc::blBrokerTemp1::blTireTemp2->get()
            ),
            keyValuePair(
                "Tire Temp 3",
                dbc::blBrokerTemp1::blTireTemp3->get()
            ),
            keyValuePair(
                "Tire Temp 4",
                dbc::blBrokerTemp2::blTireTemp4->get()
            ),
            keyValuePair(
                "Tire Temp 5",
                dbc::blBrokerTemp2::blTireTemp5->get()
            ),
            keyValuePair(
                "Tire Temp 6",
                dbc::blBrokerTemp2::blTireTemp6->get()
            ),
            keyValuePair(
                "Tire Temp 7",
                dbc::blBrokerTemp2::blTireTemp7->get()
            ),
            keyValuePair(
                "Str Gauge Uncalibrated",
                dbc::blBrokerSusUncalibrated::blStrainGaugeUncalibrated->get()
            ),
            keyValuePair(
                "Sus Pot Uncalibrated",
                dbc::blBrokerSusUncalibrated::blSusPotUncalibrated->get()
            ),
            keyValuePair(
                "Str Gauge Error",
                dbc::blBrokerCanErrorMsg::blStrainGaugeError->get()
            ),
            keyValuePair(
                "Sus Pot Error",
                dbc::blBrokerCanErrorMsg::blSusPotError->get()
            ),
            keyValuePair(
                "Tire Temp Error",
                dbc::blBrokerCanErrorMsg::blTireTempError->get()
            ),
            keyValuePair(
                "Heartbeat Count",
                dbc::blBrokerCanErrorMsg::blHeartbeatCount->get()
            ),
            keyValuePair(
                "Str Gauge Calibrated",
                dbc::blBrokerSusCalibrated::blStrainGaugeCalibrated->get()
            ),
            keyValuePair(
                "Sus Pot Calibrated",
                dbc::blBrokerSusCalibrated::blSusPotCalibrated->get()
            )
        );
        // clang-format off
    }

    okay::UIElement buildBackRightBroker() {
        // clang-format off
        return ui::growbox(okay::UIAxis::Vertical) (
            ui::h2("Back Right Broker"),
            keyValuePair(
                "Tire Temp 0",
                dbc::brBrokerTemp1::brTireTemp0->get()
            ),
            keyValuePair(
                "Tire Temp 1",
                dbc::brBrokerTemp1::brTireTemp1->get()
            ),
            keyValuePair(
                "Tire Temp 2",
                dbc::brBrokerTemp1::brTireTemp2->get()
            ),
            keyValuePair(
                "Tire Temp 3",
                dbc::brBrokerTemp1::brTireTemp3->get()
            ),
            keyValuePair(
                "Tire Temp 4",
                dbc::brBrokerTemp2::brTireTemp4->get()
            ),
            keyValuePair(
                "Tire Temp 5",
                dbc::brBrokerTemp2::brTireTemp5->get()
            ),
            keyValuePair(
                "Tire Temp 6",
                dbc::brBrokerTemp2::brTireTemp6->get()
            ),
            keyValuePair(
                "Tire Temp 7",
                dbc::brBrokerTemp2::brTireTemp7->get()
            ),
            keyValuePair(
                "Str Gauge Uncalibrated",
                dbc::brBrokerSusUncalibrated::brStrainGaugeUncalibrated->get()
            ),
            keyValuePair(
                "Sus Pot Uncalibrated",
                dbc::brBrokerSusUncalibrated::brSusPotUncalibrated->get()
            ),
            keyValuePair(
                "Str Gauge Error",
                dbc::brBrokerCanErrorMsg::brStrainGaugeError->get()
            ),
            keyValuePair(
                "Sus Pot Error",
                dbc::brBrokerCanErrorMsg::brSusPotError->get()
            ),
            keyValuePair(
                "Tire Temp Error",
                dbc::brBrokerCanErrorMsg::brTireTempError->get()
            ),
            keyValuePair(
                "Heartbeat Count",
                dbc::brBrokerCanErrorMsg::brHeartbeatCount->get()
            ),
            keyValuePair(
                "Str Gauge Calibrated",
                dbc::brBrokerSusCalibrated::brStrainGaugeCalibrated->get()
            ),
            keyValuePair(
                "Sus Pot Calibrated",
                dbc::brBrokerSusCalibrated::brSusPotCalibrated->get()
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

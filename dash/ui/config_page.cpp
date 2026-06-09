#include "config_page.hpp"

#include "car_config.hpp"
#include "inputs.hpp"
#include "materials/bat_percent.hpp"
#include "page.hpp"
#include "shared_elements.hpp"
#include "style.hpp"

#include <algorithm>
#include <can/can_dbc.hpp>
#include <memory>

namespace dash {

void ConfigPage::initializePage() {
    okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
        SharedElements::get().skyboxMaterial);
    okay::UIStyle::main().setMainFont(*fonts::latoBold);

    _selectedItem = 0;

    if (!_initializedCallbacks) {
        initializeCallbacks();
        _initializedCallbacks = true;
    }

    _entities = {
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 3),
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 3),
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 2),
        okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildLaunchControlIndicator), 1),
        okay::ecs::uiEntity(BIND_TO_THIS(buildConfig), 3),
    };
}

void ConfigPage::initializeCallbacks() {
    input::downButton.onDown([this]() {
        okay::Engine.logger.debug("Down button pushed! moving items");
        if (_selectedItem >= _sliders.size() - 1) {
            _selectedItem = 0;
        } else {
            _selectedItem++;
        }
    });

    input::upButton.onDown([this]() {
        if (_selectedItem == 0) {
            _selectedItem = _sliders.size() - 1;
        } else {
            _selectedItem--;
        }
    });

    input::encoder.onRight([this]() {
        SliderSettings& slider = _sliders[_selectedItem];
        slider.setValue(std::clamp((std::uint32_t)(slider.currentValue + slider.increment),
            slider.minValue,
            slider.maxValue));
    });

    input::encoder.onLeft([this]() {
        SliderSettings& slider = _sliders[_selectedItem];
        if (slider.increment > slider.currentValue) {
            slider.setValue(0);
        } else {
            slider.setValue(std::clamp((std::uint32_t)(slider.currentValue - slider.increment),
                slider.minValue,
                slider.maxValue));
        }
    });
}

void ConfigPage::closePage() {
    for (okay::ECSEntity& entity : _entities) {
        entity.destroy();
    }
}

okay::UIElement ConfigPage::buildConfig() {
    // clang-format off
    _sliders = {
        // Max Current Request
        (SliderSettings) {
            .sliderName = "Max Current Request",
            .units = "A",
            .minValue = 0,
            .maxValue = 360,
            .increment = 10,
            .currentValue = CarConfig::get().maxCurrentRequestRear,
            .setValue =
                [](uint32_t value) {
                    CarConfig::get().maxCurrentRequestRear = value;
                    CarConfig::get().save();
                }
        },
        // Kp
        (SliderSettings) {
            .sliderName = "Launch Control K_P",
            .units = "",
            .minValue = 5000,
            .maxValue = 12000,
            .increment = 250,
            .currentValue = CarConfig::get().launchControlKP,
            .setValue =
                [](uint32_t value) {
                    CarConfig::get().launchControlKP = value;
                    CarConfig::get().save();
                }
        },
        // Kd
        (SliderSettings) {
            .sliderName = "Launch Control K_D",
            .units = "",
            .minValue = 50,
            .maxValue = 300,
            .increment = 10,
            .currentValue = CarConfig::get().launchControlKD,
            .setValue =
                [](uint32_t value) {
                    CarConfig::get().launchControlKD = value;
                    CarConfig::get().save();
                }
        }
    };
    // clang-format on

    // clang-format off
    return ui::relFrame(0.0f, 0.0f, 1.0f, 1.0f)
        .axisSet(okay::UIAxis::Horizontal)
        .topMarginSet(100) (
            ui::spacer(),
            ui::image(*_configBg)
                .axisSet(okay::UIAxis::Vertical)
                .leftPaddingSet(25)
                .topPaddingSet(10)
                .childSpacingSet(10) (
                    ui::h1("Configuration")
                        .textSizeSet(28)
                        .fontSet(*fonts::latoBlack),
                    ui::vspacer(5),
                    ui::range(_sliders.size(), [this](std::int32_t index) {
                        return buildSlider(
                            _sliders[index],
                            index == _selectedItem
                        );
                    }),
                    ui::spacer()
            ),
            ui::hspacer(10),
            ui::image(*_vcuBg)
                .axisSet(okay::UIAxis::Vertical)
                .leftPaddingSet(25)
                .rightPaddingSet(25)
                .topPaddingSet(10) (
                    ui::h1("VCU")
                        .textSizeSet(28)
                        .fontSet(*fonts::latoBlack),
                    ui::vspacer(5),
                    ui::h3("These are values reported from VCU and are"),
                    ui::h3("actually being used to configure launch control"),
                    ui::vspacer(10),

                    keyValuePair("LC Enabled", dbc::vcuLaunchControl::lcEnabled->get()),
                    keyValuePair("LC Kp", dbc::vcuLaunchControl::lcKp->get()),
                    keyValuePair("LC Kd", dbc::vcuLaunchControl::lcKd->get()),

                    ui::spacer()
            ),
            ui::spacer()
        );
    // clang-format on
}

okay::UIElement ConfigPage::buildSlider(SliderSettings settings, bool isActive) {
    okay::MaterialHandle sliderMaterial = okay::MaterialHandle::none();
    if (_sliderMaterials.contains(settings.sliderName)) {
        sliderMaterial = _sliderMaterials.at(settings.sliderName);
    } else {
        if (!_sliderShaderHandle.isValid()) {
            _sliderShaderHandle = okay::shaderHandle(*_sliderShader);
        }

        auto sliderProps = std::make_unique<BatPercentMaterial>();
        sliderProps->isTransparent = true;
        sliderProps->useScreenspaceCoords = true;
        sliderProps->color = colors::white;
        sliderProps->albedo = *sliderTop;
        sliderProps->barColor = colors::fromHex(0xA304FFFF);
        sliderProps->bgColor = colors::fromHex(0x565150FF);

        sliderMaterial = okay::materialHandle(_sliderShaderHandle, std::move(sliderProps));
        _sliderMaterials[settings.sliderName] = sliderMaterial;
    }

    if (auto props = dynamic_cast<BatPercentMaterial*>(sliderMaterial->properties().get())) {
        props->percent = std::clamp((float)(settings.currentValue - settings.minValue) /
                                        (float)(settings.maxValue - settings.minValue),
            0.0f,
            1.0f);
    }

    // clang-format off
    return ui::box()
        .backgroundImageSet((isActive) ? *_sliderBgSelected : *_sliderBgUnslected)
        .backgroundColorSet(colors::white)
        .axisSet(okay::UIAxis::Vertical)
        .leftMarginSet((isActive) ? 10 : 0)
        (
            ui::box()
                .leftPaddingSet(16) (
                    ui::h1(settings.sliderName)
                        .textSizeSet(14)
                        .widthFixed(150)
                        .heightFixed(20)
                        .topPaddingSet(4)
                        .bottomPaddingSet(4)
                        .alignTextMiddle(),
                    ui::box()
                        .axisSet(okay::UIAxis::Horizontal)
                        .heightFixed(40)
                        .widthGrow()
                        .topPaddingSet(4)
                        .bottomPaddingSet(4) (
                            ui::h1(std::format("{} {}", settings.currentValue, settings.units))
                                .widthFixed(120)
                                .heightGrow()
                                .alignTextMiddle(),
                            ui::box()
                                .backgroundImageSet(*sliderTop)
                                .backgroundColorSet(colors::white)
                                .backgroundMaterialOverrideSet(sliderMaterial)
                                .axisSet(okay::UIAxis::Horizontal) (
                                    ui::hspacer(20),
                                    ui::h2(std::format("{}", settings.minValue))
                                        .heightGrow()
                                        .alignTextMiddle(),
                                    ui::spacer(),
                                    ui::h2(std::format("{}", settings.maxValue))
                                        .heightGrow()
                                        .alignTextMiddle(),
                                    ui::hspacer(20)
                                )
                        )
                )
        );
    // clang-format on
}

}  // namespace dash

#ifndef __CONFIG_PAGE_H__
#define __CONFIG_PAGE_H__

#include "inputs.hpp"
#include "materials/bat_percent.hpp"
#include "okay/core/ui/builder.hpp"
#include "page.hpp"
#include "shared_elements.hpp"
#include "style.hpp"

#include <okay/okay.hpp>

#include <algorithm>
#include <can/can_dbc.hpp>
#include <memory>

namespace ui = okay::ui;

namespace dash {

class ConfigPage : public IPage {
   public:
    ConfigPage() {}

    struct SliderSettings {
        const std::string& sliderName;
        uint64_t minValue;
        uint64_t maxValue;
        uint64_t currentValue;
        std::function<void(uint64_t)> setValue;
    };

    void initializePage() {
        okay::Engine.logger.debug("Creating entities for Error page!");

        okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
            SharedElements::get().skyboxMaterial);
        okay::UIStyle::main().setMainFont(*fonts::latoBold);

        _selectedItem = 0;

        if (!_initializedCallbacks) {
            initializeCallbacks();
            _initializedCallbacks = true;
        }

        _entities = {
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 1),
            okay::ecs::uiEntity(BIND_TO_THIS(buildConfig), 3),
        };
    }

    void initializeCallbacks() {
        input::downButton.onDown([this]() {
            okay::Engine.logger.debug("Down button pushed! moving items");
            if (_selectedItem >= _numItems - 1) {
                _selectedItem = 0;
            } else {
                _selectedItem++;
            }
        });

        input::upButton.onDown([this]() {
            if (_selectedItem == 0) {
                _selectedItem = _numItems - 1;
            } else {
                _selectedItem--;
            }
        });
    }

    okay::UIElement buildConfig() {
        // clang-format off
        return ui::relFrame(0.0f, 0.0f, 1.0f, 1.0f)
            .axisSet(okay::UIAxis::Vertical)
            .leftMarginSet(40)
            .rightMarginSet(40)
            .childSpacingSet(10) (
                ui::spacer(),
                // Max current request
                buildSlider(SliderSettings {
                   .sliderName = "Max Current Request",
                   .minValue = 0,
                   .maxValue = 360,
                   .currentValue = 0,
                   .setValue = [](uint64_t value) {
                       okay::Engine.logger.debug("Setting value to {}", value);
                   }
                }, _selectedItem == 0),

                // K_D
                buildSlider(SliderSettings {
                   .sliderName = "Launch Control K_D",
                   .minValue = 0,
                   .maxValue = 100,
                   .currentValue = 50,
                   .setValue = [](uint64_t value) {
                       okay::Engine.logger.debug("Setting value to {}", value);
                   }
                }, _selectedItem == 1),

                // K_P
                buildSlider(SliderSettings {
                   .sliderName = "Launch Control K_P",
                   .minValue = 0,
                   .maxValue = 100,
                   .currentValue = 100,
                   .setValue = [](uint64_t value) {
                       okay::Engine.logger.debug("Setting value to {}", value);
                   }
                }, _selectedItem == 2),

                // Slip Ratio
                buildSlider(SliderSettings {
                   .sliderName = "Slip Ratio",
                   .minValue = 0,
                   .maxValue = 100,
                   .currentValue = 75,
                   .setValue = [](uint64_t value) {
                       okay::Engine.logger.debug("Setting value to {}", value);
                   }
                }, _selectedItem == 3),

                ui::spacer()
            );
        // clang-format on
    }

    okay::UIElement buildSlider(SliderSettings settings, bool isActive) {
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
                                ui::h1(std::format("{}", settings.currentValue))
                                    .widthFixed(147)
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

    void closePage() {
        for (okay::ECSEntity& entity : _entities) {
            entity.destroy();
        }
    }

   private:
    std::vector<okay::ECSEntity> _entities;
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> _sliderBgUnslected{
        "textures/slider_bg.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> _sliderBgSelected{
        "textures/slider_bg_selected.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> sliderTop{
        "textures/slider_top.png"};

    std::unordered_map<std::string, okay::MaterialHandle> _sliderMaterials;
    okay::GameAssetRef<okay::Shader> _sliderShader{"shaders/battery"};
    okay::ShaderHandle _sliderShaderHandle{};

    std::size_t _selectedItem{0};
    const std::size_t _numItems{4};

    bool _initializedCallbacks{false};
};

}  // namespace dash

#endif

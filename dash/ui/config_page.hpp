#ifndef __CONFIG_PAGE_H__
#define __CONFIG_PAGE_H__

#include "okay/core/ui/builder.hpp"
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

        _entities = {
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 1),
            okay::ecs::uiEntity(BIND_TO_THIS(buildConfig), 3),
        };
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
                   .currentValue = 10,
                   .setValue = [](uint64_t value) {
                       okay::Engine.logger.debug("Setting value to {}", value);
                   }
                }, true),

                // Misc
                buildSlider(SliderSettings {
                   .sliderName = "Misc",
                   .minValue = 0,
                   .maxValue = 360,
                   .currentValue = 10,
                   .setValue = [](uint64_t value) {
                       okay::Engine.logger.debug("Setting value to {}", value);
                   }
                }, true),

                ui::spacer()
            );
        // clang-format on
    }

    okay::UIElement buildSlider(SliderSettings settings, bool isActive) {
        return ui::box()
            .backgroundImageSet(*sliderBg)
            .backgroundColorSet(colors::white)
            .axisSet(okay::UIAxis::Vertical)(

            );
    }

    void closePage() {
        for (okay::ECSEntity& entity : _entities) {
            entity.destroy();
        }
    }

   private:
    std::vector<okay::ECSEntity> _entities;
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> sliderBg{"textures/slider_bg.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> sliderTop{
        "textures/slider_top.png"};
};

}  // namespace dash

#endif

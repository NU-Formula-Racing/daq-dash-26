#ifndef __ERROR_PAGE_H__
#define __ERROR_PAGE_H__

#include "okay/core/ui/element.hpp"
#include "okay/core/ui/text_layout.hpp"
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

class ErrorPage : public IPage {
   public:
    ErrorPage() {}

    void initializePage() {
        okay::Engine.logger.debug("Creating entities for Error page!");

        okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
            SharedElements::get().skyboxMaterial);
        okay::UIStyle::main().setMainFont(*fonts::latoBold);

        _entities = {
            okay::ecs::uiEntity(
                []() {
                    return SharedElements::get().buildTopHud();
                },
                2),
            okay::ecs::uiEntity(
                []() {
                    return SharedElements::get().buildTemperatureElement();
                },
                2),
            okay::ecs::uiEntity(
                []() {
                    return SharedElements::get().buildBotHud();
                },
                2),
            okay::ecs::uiEntity(
                []() {
                    return SharedElements::get().buildDriveStatus();
                },
                1),
        };
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

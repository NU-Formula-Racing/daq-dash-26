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

        _entities = {
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 1),
        };
    }

    okay::UIElement buildFrontInverter1() {
        return ui::growbox(okay::UIAxis::Vertical)(

        );
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

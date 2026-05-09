#ifndef __DRIVE_PAGE_H__
#define __DRIVE_PAGE_H__

#include "okay/core/ui/builder.hpp"
#include "page.hpp"

#include <okay/okay.hpp>

namespace ui = okay::ui;

namespace dash {

class DrivePage : public IPage {
   public:
    void createEntities() {
        okay::Engine.logger.debug("Creating entities for Drive page!");

        _entities = {okay::ecs::uiEntity(buildBackground, 0),
            okay::ecs::uiEntity(buildTopHud, 1),
            okay::ecs::uiEntity(buildBotHud, 1)};
    }

    void freeEntities() {
        for (okay::ECSEntity& entity : _entities) {
            entity.destroy();
        }
    }

    static okay::UIElement buildBackground() {
        return ui::growbox().backgroundColorSet(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f})(
            ui::image(okay::load::texture("textures/bg_pattern.png")));
    }

    static okay::UIElement buildTopHud() {
        return ui::image(okay::load::texture("textures/top_bar.png"));
    }

    static okay::UIElement buildBotHud() {
        return ui::relFrame(0.0f, 0.0f, 1.0f, 1.0f)(
            ui::spacer(), ui::image(okay::load::texture("textures/bottom_bar.png")));
    }

   private:
    std::vector<okay::ECSEntity> _entities;
};

}  // namespace dash

#endif

#ifndef __DRIVE_PAGE_H__
#define __DRIVE_PAGE_H__

#include "page.hpp"

#include <okay/okay.hpp>

namespace ui = okay::ui;

namespace dash {

class DrivePage : public IPage {
   public:
    void createEntities() {
        okay::Engine.logger.debug("Creating entities for Drive page!");

        _entities = {okay::ecs::entity()
                .addComponent<okay::TransformComponent>()
                .addComponent<okay::UIComponent>([]() {
                    return ui::image(okay::load::engineTexture("uv_test.jpg"));
                })};
    }

    void freeEnties() {
        for (okay::ECSEntity& entity : _entities) {
            entity.destroy();
        }
    }

   private:
    std::vector<okay::ECSEntity> _entities;
};

}  // namespace dash

#endif

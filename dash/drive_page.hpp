#ifndef __DRIVE_PAGE_H__
#define __DRIVE_PAGE_H__

#include "okay/core/asset/asset_ref.hpp"
#include "page.hpp"

#include <okay/okay.hpp>

namespace ui = okay::ui;

namespace dash {

#define BIND_TO_THIS(fnName)   \
    [this]() {                 \
        return this->fnName(); \
    }

class DrivePage : public IPage {
   public:
    DrivePage() {}

    void createEntities() {
        okay::Engine.logger.debug("Creating entities for Drive page!");

        okay::ShaderHandle shader = okay::shaderHandle(okay::load::engineShader("shaders/lit"));

        auto materialProperties = std::make_unique<okay::LitMaterial>();
        materialProperties->color.set(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        okay::MaterialHandle material = okay::materialHandle(shader, std::move(materialProperties));

        _entities = {
            okay::ecs::uiEntity(BIND_TO_THIS(buildBackground), 0),
            okay::ecs::uiEntity(BIND_TO_THIS(buildTopHud), 1),
            okay::ecs::uiEntity(BIND_TO_THIS(buildBotHud), 1),
            okay::ecs::sceneEntity().addComponent<okay::MeshRendererComponent>(
                centerMesh, material, static_cast<uint8_t>(200)),
            okay::ecs::entity()
                .addComponent<okay::TransformComponent>(glm::vec3{0.0f, 0.0f, 5.0f})
                .addComponent<okay::CameraComponent>(
                    okay::CameraComponent{okay::Camera::PerspectiveLens{45.0f, 0.1f, 100.0f}}),

        };
    }

    void freeEntities() {
        for (okay::ECSEntity& entity : _entities) {
            entity.destroy();
        }
    }

    okay::UIElement buildBackground() {
        return ui::growbox().backgroundColorSet(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f})(
            ui::image(*bgTexture));
    }

    okay::UIElement buildTopHud() {
        return ui::image(*topBar);
    }

    okay::UIElement buildBotHud() {
        return ui::relFrame(0.0f, 0.0f, 1.0f, 1.0f)(ui::spacer(), ui::image(*botBar));
    }

   private:
    std::vector<okay::ECSEntity> _entities;

    okay::Mesh centerMesh{okay::mesh(okay::load::meshData("models/teapot.obj"))};
    okay::GameAssetRef<okay::Texture> bgTexture{"texutres/bg_pattern.png"};
    okay::GameAssetRef<okay::Texture> topBar{"texture/top_bar.png"};
    okay::GameAssetRef<okay::Texture> botBar{"texture/bottom_bar.png"};
};

}  // namespace dash

#endif

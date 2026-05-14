#ifndef __DRIVE_PAGE_H__
#define __DRIVE_PAGE_H__

#include "glm/ext/vector_float4.hpp"
#include "okay/core/asset/asset_ref.hpp"
#include "okay/core/asset/generic/texture_loader.hpp"
#include "okay/core/renderer/materials/unlit.hpp"
#include "okay/core/ui/builder.hpp"
#include "okay/core/ui/element.hpp"
#include "okay/core/ui/font.hpp"
#include "page.hpp"

#include <okay/okay.hpp>

#include <memory>

namespace ui = okay::ui;

namespace dash {

#define BIND_TO_THIS(fnName)   \
    [this]() {                 \
        return this->fnName(); \
    }

// TODO: Move this component + system into somewhere else
struct CameraControllerComponent {
    float speed{5.0f};
    float distance{10.0f};

    CameraControllerComponent() {}
    CameraControllerComponent(float speed, float distance) : speed(speed), distance(distance) {}
};

class CameraControllerSystem
    : public okay::ECSSystem<
          okay::query::Get<okay::TransformComponent, CameraControllerComponent>> {
   public:
    void onPreTick(QueryT::Item& item) override {
        auto& [transform, camController] = item.components;
        float theta =
            okay::Engine.time->timeSinceStartSec() * camController.speed * glm::pi<float>();
        glm::vec3 pos = glm::vec3(
            sin(theta) * camController.distance, 1.0f, cos(theta) * camController.distance);
        transform->position = pos;
        transform.lookAt(item.entity, glm::vec3(0.0f));
    }
};

class DrivePage : public IPage {
   public:
    DrivePage() {}

    void initializePage() {
        okay::Engine.logger.debug("Creating entities for Drive page!");

        okay::ShaderHandle objectShader =
            okay::shaderHandle(okay::load::engineShader("shaders/lit"));
        auto materialProperties = std::make_unique<okay::LitMaterial>();
        materialProperties->color.set(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        okay::MaterialHandle objectMaterial =
            okay::materialHandle(objectShader, std::move(materialProperties));

        okay::ShaderHandle skyboxShader =
            okay::shaderHandle(okay::load::shader("shaders/background"));
        auto skyboxProperties = std::make_unique<okay::UnlitMaterial>();
        skyboxProperties->albedo = *bgTexture;
        skyboxProperties->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        skyboxProperties->isTransparent = false;
        skyboxProperties->useScreenspaceCoords = true;
        skyboxProperties->doubleSided = true;
        okay::MaterialHandle skyboxMaterial =
            okay::materialHandle(skyboxShader, std::move(skyboxProperties));
        okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(skyboxMaterial);

        okay::ecs::entity()
            .addComponent<okay::TransformComponent>(glm::vec3{},
                glm::vec3{0.1f},
                glm::angleAxis(glm::radians(0.0f), glm::vec3{2.0f, 3.0f, 1.0f}))
            .addComponent<okay::LightComponent>(
                okay::LightComponent::directional(glm::vec3{1, 1, 1}, 2.5f));

        centerMesh = okay::mesh(*centerMeshData);

        okay::ecs::registerComponent<CameraControllerComponent>();
        okay::ecs::registerSystem(std::make_unique<CameraControllerSystem>());

        okay::UIStyle::main().setMainFont(*latoBold);

        _entities = {
            okay::ecs::uiEntity(BIND_TO_THIS(buildTopHud), 1),
            okay::ecs::uiEntity(BIND_TO_THIS(buildBotHud), 1),
            okay::ecs::sceneEntity().addComponent<okay::MeshRendererComponent>(
                centerMesh, objectMaterial, static_cast<uint8_t>(255)),
            okay::ecs::entity()
                .addComponent<okay::TransformComponent>(glm::vec3{0.0f, 0.0f, 20.0f})
                .addComponent<okay::CameraComponent>(
                    okay::CameraComponent{okay::Camera::PerspectiveLens{45.0f, 0.1f, 100.0f}})
                .addComponent<CameraControllerComponent>(0.25f, 60.0f),

        };
    }

    void closePage() {
        for (okay::ECSEntity& entity : _entities) {
            entity.destroy();
        }
    }

    okay::UIElement buildTopHud() {
        return ui::image(*topBar);
    }

    okay::UIElement buildBotHud() {
        const float outerPercent = 0.20f;
        const float valuePercent = 0.12f;
        const float labelPercent = 0.05f;
        const float largeFontSize = 36.0f;
        const float medFontSize = 24.0f;

        // clang-format off
        return ui::relFrame(0.0f, 0.0f, 1.0f, 1.0f)(
            ui::spacer(),
            ui::image(*botBar)
                .axisSet(okay::UIAxis::Vertical) (
                    ui::spacer(),
                    ui::flexbox()
                        .axisSet(okay::UIAxis::Horizontal)
                        .bottomMarginSet(5)
                        .widthSet(okay::size::Percent(1.0f)) (
                            ui::text("log file")
                                .widthSet(okay::size::Percent(outerPercent))
                                .textSizeSet(largeFontSize)
                                .alignTextLeft()
                                .leftMarginSet(10)
                                .fontSet(*latoBlack)
                                .alignTextBottom(),
                            ui::text("12.2")
                                .widthSet(okay::size::Percent(valuePercent))
                                .textSizeSet(medFontSize)
                                .alignTextCenter()
                                .alignTextBottom(),
                            ui::text("LV")
                                .widthSet(okay::size::Percent(labelPercent))
                                .textSizeSet(medFontSize)
                                .alignTextCenter()
                                .alignTextBottom(),
                            ui::spacer(),
                            ui::text("HV")
                                .widthSet(okay::size::Percent(labelPercent))
                                .textSizeSet(medFontSize)
                                .alignTextCenter()
                                .alignTextBottom(),
                            ui::text("480")
                                .widthSet(okay::size::Percent(valuePercent))
                                .textSizeSet(medFontSize)
                                .alignTextCenter()
                                .alignTextBottom(),
                            ui::text("milage")
                                .widthSet(okay::size::Percent(outerPercent))
                                .textSizeSet(largeFontSize)
                                .alignTextRight()
                                .rightMarginSet(10)
                                .fontSet(*latoBlack)
                                .alignTextBottom()
                        )
                )
        );
        // clang-format on
    }

   private:
    std::vector<okay::ECSEntity> _entities;

    okay::Mesh centerMesh{okay::Mesh::none()};
    okay::EngineAssetRef<okay::MeshData> centerMeshData{"models/teapot.obj"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> bgTexture{
        "textures/bg_pattern.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> topBar{"textures/top_bar.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> botBar{"textures/bottom_bar.png"};
    okay::GameAssetRef<okay::FontManager::FontHandle, okay::FontLoadOptions> latoBlack{
        "fonts/Lato-Black.ttf"};
    okay::GameAssetRef<okay::FontManager::FontHandle, okay::FontLoadOptions> latoBold{
        "fonts/Lato-Bold.ttf"};
};

}  // namespace dash

#endif

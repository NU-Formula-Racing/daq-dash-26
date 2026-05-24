#ifndef __DRIVE_PAGE_H__
#define __DRIVE_PAGE_H__

#include "components/rotate.hpp"
#include "materials/speedometer.hpp"
#include "page.hpp"
#include "shared_elements.hpp"
#include "style.hpp"

#include <okay/okay.hpp>

#include <can/can_dbc.hpp>
#include <memory>

namespace ui = okay::ui;

namespace dash {

#define BIND_TO_THIS(fnName)   \
    [this]() {                 \
        return this->fnName(); \
    }

#define LAMBDA_WRAP(fn) \
    []() {              \
        return fn();    \
    }

class DrivePage : public IPage {
   public:
    DrivePage() {}

    void initializePage() {
        okay::ShaderHandle objectShader =
            okay::shaderHandle(okay::load::engineShader("shaders/lit"));
        auto materialProperties = std::make_unique<okay::LitMaterial>();
        materialProperties->color.set(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        okay::MaterialHandle objectMaterial =
            okay::materialHandle(objectShader, std::move(materialProperties));

        okay::ecs::entity()
            .addComponent<okay::TransformComponent>(glm::vec3(),
                glm::vec3(0.1f),
                glm::angleAxis(glm::radians(-25.0f), glm::vec3(2.0f, 3.0f, 1.0f)))
            .addComponent<okay::LightComponent>(
                okay::LightComponent::directional(glm::vec3{1, 1, 1}, 2.5f));

        centerMesh = okay::mesh(*centerMeshData);

        okay::UIStyle::main().setMainFont(*fonts::latoBold);

        auto speedometerProperties = std::make_unique<SpeedometerMaterial>();
        speedometerProperties->isTransparent = true;
        speedometerProperties->useScreenspaceCoords = true;
        speedometerProperties->color = colors::white;
        speedometerProperties->albedo = *speedometerTexture;
        speedometerProperties->trailColor = colors::fromHex(0xA304FFFF);
        speedometerProperties->angle = glm::radians(45.0f);
        speedometerProperties->trailRads = glm::radians(120.0f);
        speedometerProperties->bgColor = colors::fromHex(0x342F2EFF);

        okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
            dash::SharedElements::get().skyboxMaterial);

        speedometerMaterial = okay::materialHandle(
            okay::shaderHandle(*speedometerShader), std::move(speedometerProperties));

        _entities = {
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 1),
            okay::ecs::uiEntity(BIND_TO_THIS(buildSpeedometer), 1),
            okay::ecs::entity()
                .addComponent<okay::TransformComponent>(
                    glm::vec3(0.0f), glm::vec3(0.5f), glm::identity<glm::quat>())
                .addComponent<okay::MeshRendererComponent>(
                    centerMesh, objectMaterial, static_cast<uint8_t>(255))
                .addComponent<RotateComponent>(0.25f),
            okay::ecs::entity()
                .addComponent<okay::TransformComponent>(glm::vec3{0.0f, 0.0f, 30.0f})
                .addComponent<okay::CameraComponent>(
                    okay::CameraComponent{okay::Camera::PerspectiveLens{45.0f, 0.1f, 100.0f}}),
        };
    }

    void closePage() {
        for (okay::ECSEntity& entity : _entities) {
            entity.destroy();
        }
    }

    okay::UIElement buildSpeedometer() {
        auto props = dynamic_cast<SpeedometerMaterial*>(speedometerMaterial->properties().get());
        props->angle = glm::radians(okay::Engine.time->timeSinceStartSec() * 20.0f);
        const int textWidth = 150;
        // clang-format off
        return ui::flexbox().topMarginSet(25)(
                ui::box()
                    .backgroundColorSet(colors::white)
                    .backgroundMaterialOverrideSet(speedometerMaterial)
                    .backgroundImageSet(*speedometerTexture)(
                        ui::spacer(),
                        ui::h1("75")
                            .widthSet(okay::size::Fixed(textWidth))
                            .heightFit()
                            .alignTextCenter()
                            .textSizeSet(48.0f)
                            .fontSet(*fonts::latoBlack),
                        ui::h1("MPH")
                            .widthSet(okay::size::Fixed(textWidth))
                            .heightFit()
                            .alignTextCenter()
                            .textSizeSet(20.0f),
                        ui::spacer()
                )
        );
        // clang-format on
    }

   private:
    std::vector<okay::ECSEntity> _entities;

    okay::Mesh centerMesh{okay::Mesh::none()};
    okay::EngineAssetRef<okay::MeshData> centerMeshData{"models/teapot.obj"};

    // speedometer
    okay::MaterialHandle speedometerMaterial;
    okay::GameAssetRef<okay::Shader> speedometerShader{"shaders/speedometer"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> speedometerTexture{
        "textures/speedometer.png"};
};

}  // namespace dash

#endif

#ifndef __DRIVE_PAGE_H__
#define __DRIVE_PAGE_H__

#include "components/rotate.hpp"
#include "materials/speedometer.hpp"
#include "okay/core/ui/builder.hpp"
#include "page.hpp"
#include "shared_elements.hpp"
#include "style.hpp"

#include <okay/okay.hpp>

#include <can/can_dbc.hpp>
#include <cmath>
#include <memory>

namespace ui = okay::ui;

namespace dash {

class DrivePage : public IPage {
   public:
    DrivePage() {}

    void initializePage() {
        if (centerMesh.isEmpty()) {
            okay::ShaderHandle objectShader =
                okay::shaderHandle(okay::load::engineShader("shaders/lit"));
            auto materialProperties = std::make_unique<okay::LitMaterial>();
            materialProperties->color.set(colors::fromHex(0xA304FFFF));
            materialProperties->metallic = 0.8f;
            materialProperties->anisotropic = 0.8f;
            objectMaterial = okay::materialHandle(objectShader, std::move(materialProperties));
            centerMesh = okay::mesh(*centerMeshData);
        }

        okay::UIStyle::main().setMainFont(*fonts::latoBold);

        if (!speedometerMaterial.isValid()) {
            auto speedometerProperties = std::make_unique<SpeedometerMaterial>();
            speedometerProperties->isTransparent = true;
            speedometerProperties->useScreenspaceCoords = true;
            speedometerProperties->color = colors::white;
            speedometerProperties->albedo = *speedometerTexture;
            speedometerProperties->trailColor = colors::fromHex(0xA304FFFF);
            speedometerProperties->angle = glm::radians(45.0f);
            speedometerProperties->trailRads = glm::radians(120.0f);
            speedometerProperties->bgColor = colors::fromHex(0x342F2EFF);

            speedometerMaterial = okay::materialHandle(
                okay::shaderHandle(*speedometerShader), std::move(speedometerProperties));
        }

        okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
            dash::SharedElements::get().skyboxMaterial);

        _entities = {okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 1),
            okay::ecs::uiEntity(BIND_TO_THIS(buildTemperatureDisplay), 1),
            okay::ecs::uiEntity(BIND_TO_THIS(buildSpeedometer), 1),
            okay::ecs::entity()
                .addComponent<okay::TransformComponent>(
                    glm::vec3(-4.0f, 0.0f, 0.0f), glm::vec3(0.2f), glm::identity<glm::quat>())
                .addComponent<okay::MeshRendererComponent>(
                    centerMesh, objectMaterial, static_cast<uint8_t>(255))
                .addComponent<RotateComponent>(0.25f),
            okay::ecs::entity()
                .addComponent<okay::TransformComponent>(glm::vec3{0.0f, 0.0f, 30.0f})
                .addComponent<okay::CameraComponent>(
                    okay::CameraComponent{okay::Camera::PerspectiveLens{45.0f, 0.1f, 100.0f}}),
            okay::ecs::entity()
                .addComponent<okay::TransformComponent>(glm::vec3(),
                    glm::vec3(0.1f),
                    glm::quatLookAt(
                        glm::normalize(glm::vec3(0.3f, 0.5f, -1.0f)), glm::vec3(0.0f, 1.0f, 0.0f)))
                .addComponent<okay::LightComponent>(
                    okay::LightComponent::directional(glm::vec3{1, 1, 1}, 2.5f))};
    }

    void closePage() {
        for (okay::ECSEntity& entity : _entities) {
            entity.destroy();
        }
    }

    okay::UIElement buildSpeedometer() {
        const float INCHES_PER_FOOT = 12.0f;
        const float FEET_PER_MILE = 5280.0f;
        const float SECONDS_PER_MINUTE = 60.0f;
        const float SECONDS_PER_HOUR = 60.0f * 60.0f;

        const float GEAR_RATIO = 3.4f;
        const float WHEEL_DIAMETER_INCHES = 16.0f;
        const float MAX_SPEED_MPH = 85.0f;

        float motorRpm =
            static_cast<float>(std::max<int16_t>(dbc::rearInverterMotorStatus::bRpm->get(), 0));

        float wheelRpm = motorRpm / GEAR_RATIO;

        float wheelCircumferenceInches = WHEEL_DIAMETER_INCHES * glm::pi<float>();

        float speedInchesPerSecond = wheelCircumferenceInches * (wheelRpm / SECONDS_PER_MINUTE);

        float speedMph =
            speedInchesPerSecond * SECONDS_PER_HOUR / (INCHES_PER_FOOT * FEET_PER_MILE);

        float t = glm::clamp(speedMph / MAX_SPEED_MPH, 0.0f, 1.0f);
        float angle = glm::radians(std::lerp(-90.0f, 90.0f, t));

        auto props = dynamic_cast<SpeedometerMaterial*>(speedometerMaterial->properties().get());
        props->angle = angle;
        props->trailRads = glm::radians(std::lerp(20.0f, 120.0f, t));

        const int textWidth = 150;
        // clang-format off
        return ui::flexbox().topMarginSet(25)(
                ui::box()
                    .backgroundColorSet(colors::white)
                    .backgroundMaterialOverrideSet(speedometerMaterial)
                    .backgroundImageSet(*speedometerTexture)(
                        ui::spacer(),
                        ui::h1(std::format("{:.0f}", speedMph))
                            .widthFixed(textWidth)
                            .heightFit()
                            .alignTextCenter()
                            .textSizeSet(48.0f)
                            .fontSet(*fonts::latoBlack),
                        ui::h1("MPH")
                            .widthFixed(textWidth)
                            .heightFit()
                            .alignTextCenter()
                            .textSizeSet(20.0f),
                        ui::spacer()
                )
        );
        // clang-format on
    }

    okay::UIElement buildTemperatureDisplay() {
        return ui::relCenterFrame(0.75f, 0.45f, 0.5f, 0.5f)(
            SharedElements::get().buildTemperatureElement().leftMarginSet(50));
    }

   private:
    std::vector<okay::ECSEntity> _entities;

    okay::Mesh centerMesh{okay::Mesh::none()};
    okay::GameAssetRef<okay::MeshData> centerMeshData{"models/northwestern_n.obj"};
    okay::MaterialHandle objectMaterial;

    // speedometer
    okay::MaterialHandle speedometerMaterial;
    okay::GameAssetRef<okay::Shader> speedometerShader{"shaders/speedometer"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> speedometerTexture{
        "textures/speedometer.png"};
};

}  // namespace dash

#endif

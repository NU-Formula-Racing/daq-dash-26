#ifndef __DRIVE_PAGE_H__
#define __DRIVE_PAGE_H__

#include "components/rotate.hpp"
#include "materials/speedometer.hpp"
#include "page.hpp"
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
            .addComponent<okay::TransformComponent>(glm::vec3(),
                glm::vec3(0.1f),
                glm::angleAxis(glm::radians(-25.0f), glm::vec3(2.0f, 3.0f, 1.0f)))
            .addComponent<okay::LightComponent>(
                okay::LightComponent::directional(glm::vec3{1, 1, 1}, 2.5f));

        centerMesh = okay::mesh(*centerMeshData);

        okay::ecs::registerComponent<RotateComponent>();
        okay::ecs::registerSystem(std::make_unique<RotateSystem>());

        okay::UIStyle::main().setMainFont(*latoBold);

        auto speedometerProperties = std::make_unique<SpeedometerMaterial>();
        speedometerProperties->isTransparent = true;
        speedometerProperties->useScreenspaceCoords = true;
        speedometerProperties->color = colors::white;
        speedometerProperties->albedo = *speedometerTexture;
        speedometerProperties->trailColor = colors::fromHex(0xA304FFFF);
        speedometerProperties->trailRads = glm::radians(120.0f);
        speedometerProperties->bgColor = colors::fromHex(0x342F2EFF);
        speedometerMaterial = okay::materialHandle(
            okay::shaderHandle(*speedometerShader), std::move(speedometerProperties));

        _entities = {
            okay::ecs::uiEntity(BIND_TO_THIS(buildTopHud), 2),
            okay::ecs::uiEntity(BIND_TO_THIS(buildBotHud), 2),
            okay::ecs::uiEntity(BIND_TO_THIS(buildDriveStatus), 1),
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

    okay::UIElement buildTopHud() {
        return ui::image(*topBar);
    }

    okay::UIElement buildBotHud() {
        const float outerPercent = 0.20f;
        const float valuePercent = 0.12f;
        const float labelPercent = 0.05f;
        const float largeFontSize = 32.0f;
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
                            ui::text(std::format("log_{:04}.nfr", dbc::telemetryStatus::logFile->get()))
                                .widthSet(okay::size::Percent(outerPercent))
                                .textSizeSet(largeFontSize)
                                .alignTextLeft()
                                .leftMarginSet(10)
                                .fontSet(*latoBlack)
                                .alignTextBottom(),
                            ui::text(std::format("{:.2f}", dbc::pdmBatVolt::batVolt->get()))
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
                            ui::text(std::format("{:.2f}", dbc::bmsSoe::batteryVoltage->get()))
                                .widthSet(okay::size::Percent(valuePercent))
                                .textSizeSet(medFontSize)
                                .alignTextCenter()
                                .alignTextBottom(),
                            ui::text(std::format("{:.2f} mi", dbc::telemetryOdometer::milesDriven->get()))
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

    okay::UIElement buildDriveStatus() {
        // clang-format off
        return ui::relFrame(0.0f, 0.0f, 1.0f, 1.0f)(
            ui::spacer(),
            ui::row()(
                ui::spacer(),
                ui::image(*stateShape)(ui::text(getDriveStateString())
                        .widthGrow()
                        .textSizeSet(32.0f)
                        .alignTextCenter()
                        .alignTextMiddle()
                        .fontSet(*latoBlack)
                        .topMarginSet(8))
                        .backgroundColorSet(getDriveStateColor()),
                ui::spacer()
            )
        );
        // clang-format on
    }

    okay::UIElement buildSpeedometer() {
        return ui::relFrame(0.0f, 0.0f, 1.0f, 1.0f)(ui::vspacer(10),
            ui::box()
                .backgroundColorSet(colors::white)
                .backgroundMaterialOverrideSet(speedometerMaterial)
                .backgroundImageSet(*speedometerTexture),
            ui::spacer());
    }

    std::string getDriveStateString() {
        switch (dbc::ecuDriveStatus::driveState->get()) {
            case 0:
                return "IDLE";
            case 1:
                return "PRECHARGE";
            case 2:
                return "NEUTRAL";
            case 3:
                return "DRIVE";
            default:
                return "UNKNOWN";
        }
    }

    glm::vec4 getDriveStateColor() {
        switch (dbc::ecuDriveStatus::driveState->get()) {
            case 0:
                return colors::fromHex(0x219EEBFF);
            case 1:
                return colors::fromHex(0xEBCD21FF);
            case 2:
                return colors::northwesternPurple;
            case 3:
                return colors::fromHex(0x38EB21FF);
            default:
                return colors::fromHex(0xFF00FFFF);
        }
    }

   private:
    std::vector<okay::ECSEntity> _entities;

    okay::Mesh centerMesh{okay::Mesh::none()};
    okay::EngineAssetRef<okay::MeshData> centerMeshData{"models/teapot.obj"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> bgTexture{
        "textures/bg_pattern.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> topBar{"textures/top_bar.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> botBar{"textures/bottom_bar.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> stateShape{
        "textures/state_shape.png"};
    okay::GameAssetRef<okay::FontManager::FontHandle, okay::FontLoadOptions> latoBlack{
        "fonts/Lato-Black.ttf"};
    okay::GameAssetRef<okay::FontManager::FontHandle, okay::FontLoadOptions> latoBold{
        "fonts/Lato-Bold.ttf"};

    // speedometer
    okay::MaterialHandle speedometerMaterial;
    okay::GameAssetRef<okay::Shader> speedometerShader{"shaders/speedometer"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> speedometerTexture{
        "textures/speedometer.png"};
};

}  // namespace dash

#endif

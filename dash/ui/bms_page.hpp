#ifndef __BMS_PAGE_H__
#define __BMS_PAGE_H__
#include "okay/core/asset/asset_ref.hpp"
#include "okay/core/ecs/components/render_component.hpp"
#include "okay/core/renderer/mesh.hpp"
#include "okay/core/ui/builder.hpp"
#include "okay/core/ui/element.hpp"
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

class BmsPage : public IPage {
   public:
    BmsPage() {}

    void initializePage() {
        okay::Engine.logger.debug("Creating entities for BMS page!");
        if (cellMesh.isEmpty()) {
            okay::ShaderHandle objectShader =
                okay::shaderHandle(okay::load::engineShader("shaders/lit"));
            auto materialProperties = std::make_unique<okay::LitMaterial>();
            materialProperties->color.set(colors::fromHex(0xA304FFFF));
            materialProperties->metallic = 0.8f;
            materialProperties->anisotropic = 0.8f;
            material = okay::materialHandle(objectShader, std::move(materialProperties));
            cellMesh = okay::mesh(*cellMeshData);
        }

        okay::UIStyle::main().setMainFont(*fonts::latoBold);

        okay::Engine.systems.getSystemChecked<okay::Renderer>()->setSkyboxMaterial(
            SharedElements::get().skyboxMaterial);

        _entities = {
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildTopHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildBotHud), 2),
            okay::ecs::uiEntity(LAMBDA_WRAP(SharedElements::get().buildDriveStatus), 1),
            // okay::ecs::uiEntity(BIND_TO_THIS(buildBMS), 3),
            // okay::ecs::entity()
            //     .addComponent<okay::TransformComponent>(
            //         glm::vec3(-16.0f, 6.0f, 0.0f), glm::vec3(1.0f), glm::identity<glm::quat>())
            //     .addComponent<okay::MeshRendererComponent>(
            //         cellMesh, material, static_cast<uint8_t>(255)),
            // okay::ecs::entity()
            //     .addComponent<okay::TransformComponent>(
            //         glm::vec3(-16.0f + 1.3333f, 0.0f, 0.0f), glm::vec3(1.0f), glm::identity<glm::quat>())
            //     .addComponent<okay::MeshRendererComponent>(
            //         cellMesh, material, static_cast<uint8_t>(255)),
            okay::ecs::entity()
                .addComponent<okay::TransformComponent>(glm::vec3(0.0f, 0.0f, 30.0f))
                .addComponent<okay::CameraComponent>(
                    okay::CameraComponent{okay::Camera::PerspectiveLens{45.0f, 0.1f, 100.0f}}),
                okay::ecs::entity()
                .addComponent<okay::TransformComponent>(glm::vec3(),
                    glm::vec3(0.1f),
                    glm::quatLookAt(
                        glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)), glm::vec3(0.0f, 1.0f, 0.0f)))
                .addComponent<okay::LightComponent>(
                    okay::LightComponent::directional(glm::vec3{1, 1, 1}, 2.5f))
        };
        
        float xstart = -19.5f;
        float ystart = 6.0f;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 13; j++) {
                float offset = j%2==0 ? 0.6667f : 0.0f; 
                for (int k = 0; k < 3; k++) {
                    _entities.push_back(okay::ecs::entity()
                        .addComponent<okay::TransformComponent>(
                            glm::vec3(xstart + 1.3333f * k + offset,
                                         ystart - j,
                                         0.0f))
                        .addComponent<okay::MeshRendererComponent>(
                            cellMesh, material, static_cast<uint8_t>(255)));
                }
            }
            xstart += 5;
        }
    }

    void closePage() {
        for (okay::ECSEntity& entity : _entities) {
            entity.destroy();
        }
    }

    // okay::UIElement buildBMS() {
    //     // clang-format off
    //     return ui::growbox(okay::UIAxis::Vertical) (
    //         ui::h2("Packboard"),
    //         keyValuePair(
    //             "Battery Current",
    //             dbc::bmsPackboard::batteryCurrent->get()
    //         ),
    //         keyValuePair(
    //             "Packboard Voltage",
    //             dbc::bmsPackboard::packboardVoltage->get()
    //         ),

    //         ui::vspacer(10),
    //         ui::h2("Daughterboard"),
    //         keyValuePair(
    //             "Battery Voltage",
    //             dbc::bmsDaughterboard::batteryVoltage->get()
    //         ),
    //         keyValuePair(
    //             "Max Cell Voltage",
    //             dbc::bmsDaughterboard::maxCellVoltage->get()
    //         ),
    //         keyValuePair(
    //             "Min Cell Voltage",
    //             dbc::bmsDaughterboard::minCellVoltage->get()
    //         ),
    //         keyValuePair(
    //             "Battery Temperature",
    //             dbc::bmsDaughterboard::batteryTemperature->get()
    //         ),

    //         ui::vspacer(10),
    //         ui::h2("Status"),
    //         keyValuePair(
    //             "SOC",
    //             dbc::bmsStatus::soc->get()
    //         ),
    //         keyValuePair(
    //             "BMS State",
    //             dbc::bmsStatus::bmsState->get()
    //         ),
    //         keyValuePair(
    //             "IMD State",
    //             dbc::bmsStatus::imdState->get()
    //         ),
    //         keyValuePair(
    //             "Total PEC Failures",
    //             dbc::bmsStatus::totalPecFailures->get()
    //         ),

    //         ui::vspacer(10),
    //         ui::h2("Timeouts"),
    //         keyErrorValuePair(
    //             "VCU Timeout",
    //             dbc::bmsStatus::vcuTimeout->get()
    //         ),
    //         keyErrorValuePair(
    //             "Inverter Timeout",
    //             dbc::bmsStatus::inverterTimeout->get()
    //         ),
    //         keyErrorValuePair(
    //             "Charger Timeout",
    //             dbc::bmsStatus::chargerTimeout->get()
    //         )
    //     );
    //     // clang-format on
    // }


    okay::UIElement buildContainer() {
        return ui::slot()
            .widthFixed(180)
            .heightFixed(350)
            .axisSet(okay::UIAxis::Vertical)
            .borderColorSet(colors::northwesternPurple)
            .borderRadiusSet(10)
            .borderWidthSet(5)
            .paddingSet(10)
            .backgroundColorSet(glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
    }   

    template <typename T>
    inline okay::UIElement keyValuePair(const std::string& key, const T& value) {
        // clang-format off
        return ui::slot(okay::UIAxis::Horizontal)
            .widthGrow() (
                ui::h3(key),
                ui::spacer(),
                ui::h3(std::format("{}", value))
            );
        // clang-format on
    }

    inline okay::UIElement keyErrorValuePair(const std::string& key, bool value) {
        if (value == true) {
            // clang-format off
            return ui::slot(okay::UIAxis::Horizontal)
                .widthGrow() (
                    ui::h3(key)
                        .textColorSet(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
                    ui::spacer(),
                    ui::h3(std::format("{}", value))
                        .textColorSet(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))
                );
            // clang-format on
        }

        return keyValuePair(key, value);
    }

   private:
    std::vector<okay::ECSEntity> _entities;

    okay::Mesh cellMesh{okay::Mesh::none()};
    okay::GameAssetRef<okay::MeshData> cellMeshData{"models/batterycell.obj"};
    okay::MaterialHandle material;

    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> bgTexture{
        "textures/bg_pattern.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> tempFull{"textures/temp_full.png"};
};

};  // namespace dash

#endif

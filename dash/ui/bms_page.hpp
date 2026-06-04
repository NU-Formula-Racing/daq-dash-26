#ifndef __BMS_PAGE_H__
#define __BMS_PAGE_H__
#include "okay/core/asset/asset_ref.hpp"
#include "okay/core/ecs/components/render_component.hpp"
#include "okay/core/ecs/ecs_util.hpp"
#include "okay/core/renderer/material.hpp"
#include "okay/core/renderer/materials/lit.hpp"
#include "okay/core/renderer/mesh.hpp"
#include "okay/core/ui/builder.hpp"
#include "okay/core/ui/element.hpp"
#include "okay/core/util/type.hpp"
#include "page.hpp"
#include "shared_elements.hpp"
#include "style.hpp"

#include <okay/okay.hpp>

#include <can/can_dbc.hpp>
#include <functional>
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
            okay::ecs::uiEntity(BIND_TO_THIS(buildBMSModel), 2),
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
                    okay::LightComponent::directional(glm::vec3{1, 1, 1}, 7.0f))
        };

        okay::ShaderHandle objectShader =
            okay::shaderHandle(okay::load::engineShader("shaders/lit"));

        int count = 0;
        float xstart = -12.0f;
        float ystart = 3.0f;
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 8; j++) {
                float offset = j % 2 == 0 ? 0.6667f : 0.0f; 
                for (int k = 0; k < 3; k++) {
                    if (k == 2 && j == 7) break;
                    auto props = std::make_unique<okay::LitMaterial>();
                    float volt = cellVoltages[count]->get();
                    props->color.set(getVoltColor(volt));
                    props->metallic = 0.8f;
                    props->anisotropic = 0.8f;
                    auto material = okay::materialHandle(objectShader, std::move(props));
                    _entities.push_back(okay::ecs::entity()
                        .addComponent<okay::TransformComponent>(
                            glm::vec3(xstart + 1.3333f * k + offset,
                                         ystart - j,
                                         0.0f))
                        .addComponent<okay::MeshRendererComponent>(
                            cellMesh, material, static_cast<uint8_t>(255)));
                    if (count != 114) count++;
                }
            }
            xstart += 5;
        }
    }


    okay::UIElement buildBMSModel() {
        okay::ShaderHandle objectShader =
            okay::shaderHandle(okay::load::engineShader("shaders/lit"));
        for (int i = 0; i < cellVoltages.size(); i++) {
            auto props = std::make_unique<okay::LitMaterial>();
            props->color.set(getVoltColor(cellVoltages[i]->get()));
            props->metallic = 0.8f;
            props->anisotropic = 0.8f;
            auto material = okay::materialHandle(objectShader, std::move(props));
            _entities[i + 6].getComponent<okay::MeshRendererComponent>().value().material = material;
        }
        return ui::flexbox();
    }
  

    float normalizeBatTemp(float temp) {
        const float MIN_VOLTAGE = -3.4152f;
        const float MAX_VOLTAGE = 6.41505;

        float normalized = (temp - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE);

        return glm::clamp(normalized, 0.0f, 1.0f);
    }

    glm::vec3 getVoltColor(float temp) {
        temp = normalizeBatTemp(temp);

        glm::vec3 cold(0.0f, 0.0f, 1.0f);
        glm::vec3 warm(1.0f, 1.0f, 0.0f);
        glm::vec3 hot(1.0f, 0.0f, 0.0f);

        if (temp < 0.5f) {
            float t_half = temp * 2.0f;
            return glm::mix(cold, warm, t_half);
        } else {
            float t_half = (temp - 0.5f) * 2.0f;
            return glm::mix(warm, hot, t_half);
        }
    }

    void closePage() {
        for (okay::ECSEntity& entity : _entities) {
            entity.destroy();
        }
    }

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
    static inline const std::array<CAN_Signal_FLOAT, 115> cellVoltages = {
        dbc::bmsVoltages0::cellV1,     dbc::bmsVoltages0::cellV2,
        dbc::bmsVoltages0::cellV3,     dbc::bmsVoltages0::cellV4,
        dbc::bmsVoltages1::cellV5,     dbc::bmsVoltages1::cellV6,
        dbc::bmsVoltages1::cellV7,     dbc::bmsVoltages1::cellV8,
        dbc::bmsVoltages2::cellV9,     dbc::bmsVoltages2::cellV10,
        dbc::bmsVoltages2::cellV11,    dbc::bmsVoltages2::cellV12,
        dbc::bmsVoltages3::cellV13,    dbc::bmsVoltages3::cellV14,
        dbc::bmsVoltages3::cellV15,    dbc::bmsVoltages3::cellV16,
        dbc::bmsVoltages4::cellV17,    dbc::bmsVoltages4::cellV18,
        dbc::bmsVoltages4::cellV19,    dbc::bmsVoltages4::cellV20,
        dbc::bmsVoltages5::cellV21,    dbc::bmsVoltages5::cellV22,
        dbc::bmsVoltages5::cellV23,    dbc::bmsVoltages5::cellV24,
        dbc::bmsVoltages6::cellV25,    dbc::bmsVoltages6::cellV26,
        dbc::bmsVoltages6::cellV27,    dbc::bmsVoltages6::cellV28,
        dbc::bmsVoltages7::cellV29,    dbc::bmsVoltages7::cellV30,
        dbc::bmsVoltages7::cellV31,    dbc::bmsVoltages7::cellV32,
        dbc::bmsVoltages8::cellV33,    dbc::bmsVoltages8::cellV34,
        dbc::bmsVoltages8::cellV35,    dbc::bmsVoltages8::cellV36,
        dbc::bmsVoltages9::cellV37,    dbc::bmsVoltages9::cellV38,
        dbc::bmsVoltages9::cellV39,    dbc::bmsVoltages9::cellV40,
        dbc::bmsVoltages10::cellV41,   dbc::bmsVoltages10::cellV42,
        dbc::bmsVoltages10::cellV43,   dbc::bmsVoltages10::cellV44,
        dbc::bmsVoltages11::cellV45,   dbc::bmsVoltages11::cellV46,
        dbc::bmsVoltages11::cellV47,   dbc::bmsVoltages11::cellV48,
        dbc::bmsVoltages12::cellV49,   dbc::bmsVoltages12::cellV50,
        dbc::bmsVoltages12::cellV51,   dbc::bmsVoltages12::cellV52,
        dbc::bmsVoltages13::cellV53,   dbc::bmsVoltages13::cellV54,
        dbc::bmsVoltages13::cellV55,   dbc::bmsVoltages13::cellV56,
        dbc::bmsVoltages14::cellV57,   dbc::bmsVoltages14::cellV58,
        dbc::bmsVoltages14::cellV59,   dbc::bmsVoltages14::cellV60,
        dbc::bmsVoltages15::cellV61,   dbc::bmsVoltages15::cellV62,
        dbc::bmsVoltages15::cellV63,   dbc::bmsVoltages15::cellV64,
        dbc::bmsVoltages16::cellV65,   dbc::bmsVoltages16::cellV66,
        dbc::bmsVoltages16::cellV67,   dbc::bmsVoltages16::cellV68,
        dbc::bmsVoltages17::cellV69,   dbc::bmsVoltages17::cellV70,
        dbc::bmsVoltages17::cellV71,   dbc::bmsVoltages17::cellV72,
        dbc::bmsVoltages18::cellV73,   dbc::bmsVoltages18::cellV74,
        dbc::bmsVoltages18::cellV75,   dbc::bmsVoltages18::cellV76,
        dbc::bmsVoltages19::cellV77,   dbc::bmsVoltages19::cellV78,
        dbc::bmsVoltages19::cellV79,   dbc::bmsVoltages19::cellV80,
        dbc::bmsVoltages20::cellV81,   dbc::bmsVoltages20::cellV82,
        dbc::bmsVoltages20::cellV83,   dbc::bmsVoltages20::cellV84,
        dbc::bmsVoltages21::cellV85,   dbc::bmsVoltages21::cellV86,
        dbc::bmsVoltages21::cellV87,   dbc::bmsVoltages21::cellV88,
        dbc::bmsVoltages22::cellV89,   dbc::bmsVoltages22::cellV90,
        dbc::bmsVoltages22::cellV91,   dbc::bmsVoltages22::cellV92,
        dbc::bmsVoltages23::cellV93,   dbc::bmsVoltages23::cellV94,
        dbc::bmsVoltages23::cellV95,   dbc::bmsVoltages23::cellV96,
        dbc::bmsVoltages24::cellV97,   dbc::bmsVoltages24::cellV98,
        dbc::bmsVoltages24::cellV99,   dbc::bmsVoltages24::cellV100,
        dbc::bmsVoltages25::cellV101,  dbc::bmsVoltages25::cellV102,
        dbc::bmsVoltages25::cellV103,  dbc::bmsVoltages25::cellV104,
        dbc::bmsVoltages26::cellV105,  dbc::bmsVoltages26::cellV106,
        dbc::bmsVoltages26::cellV107,  dbc::bmsVoltages26::cellV108,
        dbc::bmsVoltages27::cellV109,  dbc::bmsVoltages27::cellV110,
        dbc::bmsVoltages27::cellV111,  dbc::bmsVoltages27::cellV112,
        dbc::bmsVoltages28::cellV113,  dbc::bmsVoltages28::cellV114,
        dbc::bmsVoltages28::cellV115,
    };

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

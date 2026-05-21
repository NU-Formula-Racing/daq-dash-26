#ifndef __ERROR_PAGE_H__
#define __ERROR_PAGE_H__

#include "page.hpp"
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

class ErrorPage : public IPage {
   public:
    ErrorPage() {}

    void initializePage() {
        okay::Engine.logger.debug("Creating entities for Error page!");

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

        okay::UIStyle::main().setMainFont(*latoBold);

        _entities = {
            okay::ecs::uiEntity(dash::BIND_TO_THIS(buildTopHud), 2),
            okay::ecs::uiEntity(dash::BIND_TO_THIS(buildBotHud), 2),
            okay::ecs::uiEntity(dash::BIND_TO_THIS(buildDriveStatus), 1),
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
};

}  // namespace dash

#endif

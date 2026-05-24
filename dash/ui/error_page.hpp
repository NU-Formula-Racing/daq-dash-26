#ifndef __ERROR_PAGE_H__
#define __ERROR_PAGE_H__

#include "okay/core/ui/builder.hpp"
#include "okay/core/ui/element.hpp"
#include "okay/core/ui/text_layout.hpp"
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
            okay::ecs::uiEntity(BIND_TO_THIS(buildTopHud), 2),
            okay::ecs::uiEntity(BIND_TO_THIS(buildContent), 2),
            okay::ecs::uiEntity(BIND_TO_THIS(buildDriveStatus), 1),
        };
    }

    void closePage() {
        for (okay::ECSEntity& entity : _entities) {
            entity.destroy();
        }
    }

    okay::UIElement buildTopHud() {
        const float largeFontSize = 32.0f;

        return ui::relFrame(0.0f, 0.0f, 1.0f, 0.125f) (
            ui::image(*topBar) (
                ui::text(std::format("log_{:04}.nfr", dbc::telemetryStatus::logFile->get()))
                    .widthSet(okay::size::Percent(1.0f))
                    .textSizeSet(largeFontSize)
                    .alignTextCenter()
                    .fontSet(*latoBlack)
                    .topMarginSet(okay::size::Fixed(10))
            )
        );
    }

    okay::UIElement buildContent() {
        const float largeFontSize = 32.0f;
        const float medFontSize = 24.0f;
        const float smallFontSize = 16.0f;

        return ui::relFrame(0.0625f, 0.21, 0.9375f, 0.71)(
            ui::flexbox()
                .axisSet(okay::UIAxis::Horizontal)
                .widthGrow()
                .heightGrow() (
                    // Error list
                    ui::flexbox()
                        .axisSet(okay::UIAxis::Vertical)
                        .widthGrow()
                        .heightGrow() (
                            ui::image(*errorCodeTop) (
                                    ui::text("BMS")
                                        .widthGrow()
                                        .textSizeSet(medFontSize)
                                        .leftMarginSet(15)
                                ),
                            ui::image(*errorCodeMid) (
                                    ui::text("OUR_VOLT")
                                        .widthGrow()
                                        .textSizeSet(medFontSize)
                                        .leftMarginSet(15)
                                ),
                            ui::image(*errorCodeMid) (
                                    ui::text("OUR_TEMP")
                                        .widthGrow()
                                        .textSizeSet(medFontSize)
                                        .leftMarginSet(15)
                                ),
                            ui::image(*errorCodeBot),
                            ui::spacer()
                                .widthSet(okay::size::Fixed(25)),
                            ui::image(*errorCodeTop) (
                                    ui::text("ECU")
                                        .widthGrow()
                                        .textSizeSet(medFontSize)
                                        .leftMarginSet(15)
                                ),
                            ui::image(*errorCodeMid) (
                                    ui::text("IMPLS_PRSNT")
                                        .widthGrow()
                                        .textSizeSet(medFontSize)
                                        .leftMarginSet(15)
                                ),
                            ui::image(*errorCodeMid) (
                                    ui::text("IMPLS_BRK_INVLD_IMP")
                                        .widthGrow()
                                        .textSizeSet(medFontSize)
                                        .leftMarginSet(15)
                                ),
                            ui::image(*errorCodeMid) (
                                    ui::text("IMPLS_APPS_INVLD_IMP")
                                        .widthGrow()
                                        .textSizeSet(medFontSize)
                                        .leftMarginSet(15)
                                ),
                            ui::image(*errorCodeBot),
                            ui::spacer()
                        ),
                    // Temperature info
                    ui::flexbox()
                        .axisSet(okay::UIAxis::Vertical)
                        .widthGrow()
                        .heightGrow()
                        .topMarginSet(okay::size::Fixed(10)) (
                            ui::image(*tempFull) (
                                ui::spacer(),
                                ui::text("CELL")
                                    .widthGrow()
                                    .textSizeSet(largeFontSize)
                                    .alignTextCenter(),
                                ui::spacer(),
                                ui::flexbox()
                                    .axisSet(okay::UIAxis::Horizontal)
                                    .widthGrow()
                                    .heightSet(okay::size::Fixed(64))
                                    .bottomMarginSet(8) (
                                        ui::flexbox()
                                            .axisSet(okay::UIAxis::Vertical)
                                            .widthGrow()
                                            .heightGrow() (
                                                ui::spacer(),
                                                ui::text("minimum")
                                                    .widthGrow()
                                                    .textSizeSet(smallFontSize)
                                                    .alignTextCenter(),
                                                ui::text("3.79")
                                                    .widthGrow()
                                                    .textSizeSet(largeFontSize)
                                                    .alignTextCenter(),
                                                ui::spacer()
                                            ),
                                        ui::spacer()
                                            .widthSet(okay::size::Fixed(14)),
                                        ui::flexbox()
                                            .axisSet(okay::UIAxis::Vertical)
                                            .widthGrow()
                                            .heightGrow() (
                                                ui::spacer(),
                                                ui::text("maximum")
                                                    .widthGrow()
                                                    .textSizeSet(smallFontSize)
                                                    .alignTextCenter(),
                                                ui::text("19")
                                                    .widthGrow()
                                                    .textSizeSet(largeFontSize)
                                                    .alignTextCenter(),
                                                ui::spacer()
                                            )
                                    ),
                                ui::spacer()
                                    .widthSet(okay::size::Fixed(20)),
                                ui::text("IGBT")
                                    .widthGrow()
                                    .textSizeSet(largeFontSize)
                                    .alignTextCenter()
                                    .bottomMarginSet(10),
                                ui::spacer(),
                                ui::flexbox()
                                    .axisSet(okay::UIAxis::Horizontal)
                                    .widthGrow()
                                    .heightSet(okay::size::Fixed(64)) (
                                        ui::flexbox()
                                            .axisSet(okay::UIAxis::Vertical)
                                            .widthGrow()
                                            .heightGrow() (
                                                ui::spacer(),
                                                ui::text("left")
                                                    .widthGrow()
                                                    .heightFit()
                                                    .textSizeSet(smallFontSize)
                                                    .alignTextCenter(),
                                                ui::text("3.79")
                                                    .widthGrow()
                                                    .heightFit()
                                                    .textSizeSet(largeFontSize)
                                                    .alignTextCenter(),
                                                ui::spacer()
                                            ),
                                        ui::spacer()
                                            .widthSet(okay::size::Fixed(6)),
                                        ui::flexbox()
                                            .axisSet(okay::UIAxis::Vertical)
                                            .widthGrow()
                                            .heightGrow() (
                                                ui::spacer(),
                                                ui::text("rear")
                                                    .widthGrow()
                                                    .heightFit()
                                                    .textSizeSet(smallFontSize)
                                                    .alignTextTop()
                                                    .alignTextCenter(),
                                                ui::text("19")
                                                    .widthGrow()
                                                    .heightFit()
                                                    .textSizeSet(largeFontSize)
                                                    .alignTextCenter(),
                                                ui::spacer()
                                            ),
                                        ui::spacer()
                                            .widthSet(okay::size::Fixed(6)),
                                        ui::flexbox()
                                            .axisSet(okay::UIAxis::Vertical)
                                            .widthGrow()
                                            .heightGrow() (
                                                ui::spacer(),
                                                ui::text("right")
                                                    .widthGrow()
                                                    .heightFit()
                                                    .textSizeSet(smallFontSize)
                                                    .alignTextCenter(),
                                                ui::text("19")
                                                    .widthGrow()
                                                    .heightFit()
                                                    .textSizeSet(largeFontSize)
                                                    .alignTextCenter(),
                                                ui::spacer()
                                            )
                                    )
                                )
                        )
                )
        );
    }

    okay::UIElement buildDriveStatus() {
        // clang-format off
        return ui::relFrame(0.0f, 0.0f, 1.0f, 1.0f)(
            ui::spacer(),
            ui::row()(
                ui::spacer(),
                ui::image(*stateShape)(ui::text("FAULT")
                        .widthGrow()
                        .textSizeSet(32.0f)
                        .alignTextCenter()
                        .alignTextMiddle()
                        .fontSet(*latoBlack)
                        .topMarginSet(8))
                        .backgroundColorSet(colors::fromHex(0xEB2121FF)),
                ui::spacer()
            )
        );
        // clang-format on
    }

   private:
    std::vector<okay::ECSEntity> _entities;

    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> bgTexture{
        "textures/bg_pattern.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> topBar{"textures/top_bar.png"};

    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> errorCodeTop{"textures/error_code_top.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> errorCodeMid{"textures/error_code_mid.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> errorCodeBot{"textures/error_code_bot.png"};

    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> tempFull{"textures/temp_full.png"};

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

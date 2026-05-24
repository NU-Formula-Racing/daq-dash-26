#ifndef __SHARED_ELEMENTS_H__
#define __SHARED_ELEMENTS_H__

#include "materials/bat_percent.hpp"
#include "style.hpp"

#include <okay/okay.hpp>

#include <can/can_dbc.hpp>
#include <memory>

namespace ui = okay::ui;

namespace dash {

class SharedElements {
   public:
    static SharedElements& get() {
        static SharedElements elements;
        return elements;
    }

    SharedElements() {
        // Skybox shaer
        okay::ShaderHandle skyboxShader =
            okay::shaderHandle(okay::load::shader("shaders/background"));
        auto skyboxProperties = std::make_unique<okay::UnlitMaterial>();
        skyboxProperties->albedo = *bgTexture;
        skyboxProperties->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        skyboxProperties->isTransparent = false;
        skyboxProperties->useScreenspaceCoords = true;
        skyboxProperties->doubleSided = true;
        skyboxMaterial = okay::materialHandle(skyboxShader, std::move(skyboxProperties));

        auto batPercentProperties = std::make_unique<BatPercentMaterial>();
        batPercentProperties->isTransparent = true;
        batPercentProperties->useScreenspaceCoords = true;
        batPercentProperties->color = colors::white;
        batPercentProperties->albedo = *batPerecentBG;
        batPercentProperties->barColor = colors::fromHex(0xA304FFFF);
        batPercentProperties->bgColor = colors::fromHex(0x565150FF);

        batPercentMaterial = okay::materialHandle(
            okay::shaderHandle(*batPercentShader), std::move(batPercentProperties));
    }

    okay::UIElement buildTopHud() {
        auto props = dynamic_cast<BatPercentMaterial*>(batPercentMaterial->properties().get());
        props->percent = dbc::bmsStatus::bmsSoc->get();
        // clang-format off
        return ui::image(*topBar)
            .axisSet(okay::UIAxis::Horizontal) (
                ui::spacer(),
                ui::flexbox()
                    .axisSet(okay::UIAxis::Vertical)
                    .heightGrow()
                    (
                        ui::spacer(),
                        ui::image(*batPerecentBG)
                            .backgroundColorSet(colors::white)
                            .backgroundMaterialOverrideSet(batPercentMaterial)
                            .axisSet(okay::UIAxis::Horizontal) (
                                ui::spacer(),
                                ui::image(*batPerecentOverlay)
                                    .textSet(std::format("{:.1f}%", dbc::bmsStatus::bmsSoc->get() * 100.0f))
                                    .alignTextCenter()
                                    .alignTextMiddle()
                                    .fontSet(*fonts::latoBlack)
                                    .textSizeSet(24.0f),
                                ui::spacer()
                            ),
                         ui::spacer()
                    ),
                ui::spacer()
            );
        // clang-format on
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
                                .fontSet(*fonts::latoBlack)
                                .alignTextBottom()
                                .heightGrow(),
                            ui::text(std::format("{:.2f}", dbc::pdmBatVolt::batVolt->get()))
                                .widthSet(okay::size::Percent(valuePercent))
                                .textSizeSet(medFontSize)
                                .alignTextCenter()
                                .alignTextBottom()
                                .heightGrow(),
                            ui::text("LV")
                                .widthSet(okay::size::Percent(labelPercent))
                                .textSizeSet(medFontSize)
                                .alignTextCenter()
                                .alignTextBottom()
                                .heightGrow(),
                            ui::spacer(),
                            ui::text("HV")
                                .widthSet(okay::size::Percent(labelPercent))
                                .textSizeSet(medFontSize)
                                .alignTextCenter()
                                .alignTextBottom()
                                .heightGrow(),
                            ui::text(std::format("{:.2f}", dbc::bmsSoe::batteryVoltage->get()))
                                .widthSet(okay::size::Percent(valuePercent))
                                .textSizeSet(medFontSize)
                                .alignTextCenter()
                                .alignTextBottom()
                                .heightGrow(),
                            ui::text(std::format("{:.2f} mi", dbc::telemetryOdometer::milesDriven->get()))
                                .widthSet(okay::size::Percent(outerPercent))
                                .textSizeSet(largeFontSize)
                                .alignTextRight()
                                .rightMarginSet(10)
                                .fontSet(*fonts::latoBlack)
                                .alignTextBottom()
                                .heightGrow()
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
                        .heightGrow()
                        .textSizeSet(32.0f)
                        .alignTextCenter()
                        .alignTextMiddle()
                        .fontSet(*fonts::latoBlack)
                        .topMarginSet(8))
                        .backgroundColorSet(getDriveStateColor()),
                ui::spacer()
            )
        );
        // clang-format on
    }

    okay::UIElement buildTemperatureElement() {
        const float largeFontSize = 32.0f;
        const float medFontSize = 24.0f;
        const float smallFontSize = 16.0f;

        // clang-format off
        return ui::image(*tempFull)(
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
                .bottomMarginSet(8)(
                    ui::flexbox()
                        .axisSet(okay::UIAxis::Vertical)
                        .widthGrow()
                        .heightGrow()(
                            ui::spacer(),
                            ui::text("minimum")
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
                        .widthSet(okay::size::Fixed(14)),
                    ui::flexbox()
                        .axisSet(okay::UIAxis::Vertical)
                        .widthGrow()
                        .heightGrow()(
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
                            .heightGrow()(
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
                        .heightGrow()(
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
                        .heightGrow()(
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
                            ui::spacer())));

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

    okay::MaterialHandle skyboxMaterial;

    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> bgTexture{
        "textures/bg_pattern.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> topBar{"textures/top_bar.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> botBar{"textures/bottom_bar.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> stateShape{
        "textures/state_shape.png"};

    // Battery Percentage
    okay::MaterialHandle batPercentMaterial;
    okay::GameAssetRef<okay::Shader> batPercentShader{"shaders/battery"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> batPerecentBG{
        "textures/bat_percent_bg.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> batPerecentOverlay{
        "textures/bat_percent_over.png"};

    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> tempFull{"textures/temp_full.png"};
};

};  // namespace dash

#endif  // __SHARED_ELEMENTS_H__

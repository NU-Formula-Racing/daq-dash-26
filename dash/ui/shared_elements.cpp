#include "shared_elements.hpp"

namespace ui = okay::ui;

namespace dash {

SharedElements::SharedElements() {
    // Skybox shader
    okay::ShaderHandle skyboxShader = okay::shaderHandle(okay::load::shader("shaders/background"));
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

    auto launchControlIndicatorProperties = std::make_unique<LaunchControlMaterial>();
    launchControlIndicatorProperties->vignetteOuterColor = colors::northwesternPurple;
    launchControlIndicatorProperties->vignetteInnerColor = colors::fromHex(0xA304FFFF);
    launchControlIndicatorProperties->color = colors::white;
    launchControlIndicatorProperties->isTransparent = true;
    launchControlIndicatorProperties->useScreenspaceCoords = true;

    launchControlIndicatorMaterial =
        okay::materialHandle(okay::shaderHandle(*launchControlIndicatorShader),
            std::move(launchControlIndicatorProperties));
}

okay::UIElement SharedElements::buildTopHud() {
    auto props = dynamic_cast<BatPercentMaterial*>(batPercentMaterial->properties().get());
    props->percent = dbc::bmsStatus::soc->get();
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
                                .textSet(std::format("{:.1f}%", dbc::bmsStatus::soc->get() * 100.0f))
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

okay::UIElement SharedElements::buildBotHud() {
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
                        ui::text(std::format("{:.2f}", dbc::bmsPackboard::packboardVoltage->get()))
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

okay::UIElement SharedElements::buildDriveStatus() {
    // clang-format off
    return ui::relFrame(0.0f, 0.0f, 1.0f, 1.0f)(
        ui::spacer(),
        ui::row()(
            ui::spacer(),
            ui::image(*stateShape)(ui::text(CarState::getDriveStateString())
                    .widthGrow()
                    .heightGrow()
                    .textSizeSet(32.0f)
                    .alignTextCenter()
                    .alignTextMiddle()
                    .fontSet(*fonts::latoBlack)
                    .topMarginSet(8))
                    .backgroundColorSet(CarState::getDriveStateColor()),
            ui::spacer()
        )
    );
    // clang-format on
}

okay::UIElement SharedElements::buildTemperatureElement() {
    const float largeFontSize = 32.0f;
    const float medFontSize = 24.0f;
    const float smallFontSize = 16.0f;

    // clang-format off
    return ui::flexbox()
        .axisSet(okay::UIAxis::Vertical)
        .widthGrow()
        .heightGrow()
        .topMarginSet(okay::size::Fixed(10)) (
            ui::image(*tempFull) (
                ui::spacer(),
                ui::text("BATTERY")
                    .widthGrow()
                    .heightGrow()
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
                                ui::text("current")
                                    .widthGrow()
                                    .heightGrow()
                                    .textSizeSet(smallFontSize)
                                    .alignTextCenter(),
                                ui::text(std::format("{:.1f} A", dbc::bmsPackboard::batteryCurrent->get()))
                                    .widthGrow()
                                    .heightGrow()
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
                                ui::text("temp")
                                    .widthGrow()
                                    .heightGrow()
                                    .textSizeSet(smallFontSize)
                                    .alignTextCenter(),
                                ui::text(std::format("{:.1f} C", dbc::bmsDaughterboard::batteryTemperature->get()))
                                    .widthGrow()
                                    .heightGrow()
                                    .textSizeSet(largeFontSize)
                                    .alignTextCenter(),
                                ui::spacer()
                            )
                    ),
                ui::spacer()
                    .widthSet(okay::size::Fixed(20)),
                ui::text("INVERTER")
                    .widthGrow()
                    .heightGrow()
                    .textSizeSet(largeFontSize)
                    .alignTextCenter()
                    .bottomMarginSet(16),
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
                                ui::text(std::format("{} C", dbc::frontLeftInverterTempStatus::flIgbtTemp->get()))
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
                                ui::text(std::format("{} C", dbc::rearInverterTempStatus::bIgbtTemp->get()))
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
                                ui::text(std::format("{} C", dbc::frontRightInverterTempStatus::frIgbtTemp->get()))
                                    .widthGrow()
                                    .heightFit()
                                    .textSizeSet(largeFontSize)
                                    .alignTextCenter(),
                                ui::spacer()
                            )
                    )
                )
        );

    // clang-format on
}

okay::UIElement SharedElements::buildLaunchControlIndicator() {
    if (!CarConfig::get().enableLaunchControl && !dbc::vcuLaunchControl::lcEnabled->get()) {
        return ui::relFrame(0.0f, 0.0f, 1.0f, 1.0f);
    }

    glm::vec4 outerColor = colors::fromHex(0xA304FFFF);
    glm::vec4 innerColor = colors::northwesternPurple;
    float thickness = 2.0f;

    if (!dbc::vcuLaunchControl::lcEnabled->get()) {
        outerColor = colors::fromHex(0x9C9C9CFF);
        innerColor = colors::white;
        thickness = 1.5;
    }

    if (auto* props = dynamic_cast<LaunchControlMaterial*>(
            launchControlIndicatorMaterial->properties().get())) {
        props->vignetteInnerColor = innerColor;
        props->vignetteOuterColor = outerColor;
        props->speedLineThickness = thickness;
    }

    return ui::relFrame(0.0f, 0.0f, 1.0f, 1.0f)
        .backgroundColorSet(colors::white)
        .backgroundMaterialOverrideSet(launchControlIndicatorMaterial);
}

}  // namespace dash

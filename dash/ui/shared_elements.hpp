#ifndef __SHARED_ELEMENTS_H__
#define __SHARED_ELEMENTS_H__

#include "car_config.hpp"
#include "car_state.hpp"
#include "materials/bat_percent.hpp"
#include "materials/launch_control.hpp"
#include "okay/core/ui/element.hpp"
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

    SharedElements();

    okay::UIElement buildTopHud();
    okay::UIElement buildBotHud();
    okay::UIElement buildDriveStatus();
    okay::UIElement buildTemperatureElement();
    okay::UIElement buildPerformanceUI();
    okay::UIElement buildLaunchControlIndicator();

    okay::MaterialHandle skyboxMaterial;
    okay::MaterialHandle launchControlIndicatorMaterial;

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

    // Launch control
    okay::GameAssetRef<okay::Shader> launchControlIndicatorShader{"shaders/launch_control"};
};

};  // namespace dash

#endif  // __SHARED_ELEMENTS_H__

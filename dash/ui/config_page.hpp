#ifndef __CONFIG_PAGE_H__
#define __CONFIG_PAGE_H__

#include "page.hpp"

#include <okay/okay.hpp>

namespace ui = okay::ui;

namespace dash {

class ConfigPage : public IPage {
   public:
    ConfigPage() {}

    struct SliderSettings {
        const char* sliderName;
        const char* units;
        uint32_t minValue;
        uint32_t maxValue;
        uint32_t increment;
        uint32_t currentValue;
        std::function<void(uint32_t)> setValue;
    };

    void initializePage();
    void closePage();

   private:
    void initializeCallbacks();
    okay::UIElement buildConfig();
    okay::UIElement buildSlider(SliderSettings settings, bool isActive);

    template <typename T>
    inline okay::UIElement keyValuePair(const std::string& key, const T& value) {
        // clang-format off
        return ui::slot(okay::UIAxis::Horizontal)
            .widthGrow() (
                ui::h1(key),
                ui::spacer(),
                ui::h1(std::format("{}", value))
            );
        // clang-format on
    }

    std::vector<okay::ECSEntity> _entities;
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> _sliderBgUnslected{
        "textures/slider_bg.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> _sliderBgSelected{
        "textures/slider_bg_selected.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> sliderTop{
        "textures/slider_top.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> _configBg{
        "textures/config_bg.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> _vcuBg{
        "textures/config_vcu_bg.png"};

    std::unordered_map<std::string, okay::MaterialHandle> _sliderMaterials;
    okay::GameAssetRef<okay::Shader> _sliderShader{"shaders/battery"};
    okay::ShaderHandle _sliderShaderHandle{};

    std::size_t _selectedItem{0};
    bool _initializedCallbacks{false};

    std::vector<SliderSettings> _sliders;
};

}  // namespace dash

#endif

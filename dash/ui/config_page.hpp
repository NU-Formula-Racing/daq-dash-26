#ifndef __CONFIG_PAGE_H__
#define __CONFIG_PAGE_H__

#include "page.hpp"

#include <okay/okay.hpp>

namespace dash {

class ConfigPage : public IPage {
   public:
    ConfigPage() {}

    struct SliderSettings {
        const char* sliderName;
        uint16_t minValue;
        uint16_t maxValue;
        uint16_t increment;
        uint16_t currentValue;
        std::function<void(uint16_t)> setValue;
    };

    void initializePage();
    void closePage();

   private:
    void initializeCallbacks();
    okay::UIElement buildConfig();
    okay::UIElement buildSlider(SliderSettings settings, bool isActive);

    std::vector<okay::ECSEntity> _entities;
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> _sliderBgUnslected{
        "textures/slider_bg.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> _sliderBgSelected{
        "textures/slider_bg_selected.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> sliderTop{
        "textures/slider_top.png"};

    std::unordered_map<std::string, okay::MaterialHandle> _sliderMaterials;
    okay::GameAssetRef<okay::Shader> _sliderShader{"shaders/battery"};
    okay::ShaderHandle _sliderShaderHandle{};

    std::size_t _selectedItem{0};
    bool _initializedCallbacks{false};

    std::vector<SliderSettings> _sliders;
};

}  // namespace dash

#endif

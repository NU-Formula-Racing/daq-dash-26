#ifndef __DEBUG_PAGE_H__
#define __DEBUG_PAGE_H__

#include "page.hpp"

#include <okay/okay.hpp>

#include <vector>

namespace ui = okay::ui;

namespace dash {

class DebugPage : public IPage {
   public:
    DebugPage() {}

    void initializePage();
    void closePage();

   private:
    okay::UIElement buildDebug();
    okay::UIElement buildContainer();
    okay::UIElement buildVCU();
    okay::UIElement buildInverter();
    okay::UIElement buildBMS();
    okay::UIElement buildFaults();

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

    std::vector<okay::ECSEntity> _entities;
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> bgTexture{
        "textures/bg_pattern.png"};
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> tempFull{"textures/temp_full.png"};
};

}  // namespace dash

#endif

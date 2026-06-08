#ifndef __INVERTER_PAGE_H__
#define __INVERTER_PAGE_H__

#include "page.hpp"

#include <okay/okay.hpp>

#include <vector>

namespace ui = okay::ui;

namespace dash {

class InverterPage : public dash::IPage {
   public:
    InverterPage() {}

    void initializePage();
    void closePage();

   private:
    okay::UIElement buildDebug();
    okay::UIElement buildContainer();
    okay::UIElement buildFrontRightInverter();
    okay::UIElement buildFrontLeftInverter();
    okay::UIElement buildRearInverter();
    okay::UIElement buildVCUInverter();

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

    std::vector<okay::ECSEntity> _entities;
};

}  // namespace dash

#endif

#ifndef __IMU_PDM_TLM_DEBUG_PAGE_H__
#define __IMU_PDM_TLM_DEBUG_PAGE_H__

#include "page.hpp"

#include <okay/okay.hpp>

namespace ui = okay::ui;

namespace dash {

class IMUPDMTLMDebugPage : public IPage {
   public:
    IMUPDMTLMDebugPage() {}

    void initializePage();
    void closePage();

   private:
    okay::UIElement buildIMUPDMTLMDebug();
    okay::UIElement buildContainer();
    okay::UIElement buildIMU1();
    okay::UIElement buildIMU2();
    okay::UIElement buildPDM();
    okay::UIElement buildTelemetry();

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

   private:
    std::vector<okay::ECSEntity> _entities;
};

}  // namespace dash

#endif

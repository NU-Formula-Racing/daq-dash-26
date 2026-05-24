#ifndef __BAT_PERCENT_H__
#define __BAT_PERCENT_H__

#include <okay/okay.hpp>

#include <tuple>

namespace dash {

struct BatPercentMaterial : public okay::UIRectProperties,
                            okay::OkayMaterialProperties<BatPercentMaterial> {
   public:
    okay::UniformProperty<float, okay::FixedString("u_percent")> percent{0.0f};
    okay::UniformProperty<glm::vec4, okay::FixedString("u_barColor")> barColor{glm::vec4(1.0f)};
    okay::UniformProperty<glm::vec4, okay::FixedString("u_bgColor")> bgColor{glm::vec4(1.0f)};

    auto uniformRefs() {
        return std::tuple_cat(
            okay::UIRectProperties::uniformRefs(), std::tie(percent, barColor, bgColor));
    }

    auto uniformRefs() const {
        return std::tuple_cat(
            okay::UIRectProperties::uniformRefs(), std::tie(percent, barColor, bgColor));
    }

    auto uniformBlockRefs() const {
        return okay::UIRectProperties::uniformBlockRefs();
    }

    auto textureRefs() {
        return okay::UIRectProperties::textureRefs();
    }

    auto textureRefs() const {
        return okay::UIRectProperties::textureRefs();
    }

    okay::MaterialFlagCollection flags() {
        okay::MaterialFlagCollection flags = okay::SceneMaterialProperties::flags();
        flags.addFlag(okay::MaterialFlags::UNLIT);
        return flags;
    }
};

}  // namespace dash

#endif  // __BAT_PERCENT_H__

#ifndef __SPEEDOMETER_H__
#define __SPEEDOMETER_H__

#include <okay/okay.hpp>

#include <tuple>

namespace dash {

struct SpeedometerMaterial : public okay::UIRectProperties,
                             okay::OkayMaterialProperties<SpeedometerMaterial> {
   public:
    okay::UniformProperty<float, okay::FixedString("u_angle")> angle{0.0f};
    okay::UniformProperty<float, okay::FixedString("u_trailRads")> trailRads{glm::radians(30.0f)};
    okay::UniformProperty<glm::vec4, okay::FixedString("u_trailColor")> trailColor{glm::vec4(1.0f)};
    okay::UniformProperty<glm::vec4, okay::FixedString("u_needColor")> needleColor{glm::vec4(1.0f)};

    auto uniformRefs() {
        return std::tuple_cat(okay::UIRectProperties::uniformRefs(),
            std::tie(angle, trailRads, trailColor, needleColor));
    }

    auto uniformRefs() const {
        return std::tuple_cat(okay::UIRectProperties::uniformRefs(),
            std::tie(angle, trailRads, trailColor, needleColor));
    }

    auto uniformBlockRefs() const {
        return UIRectProperties::uniformBlockRefs();
    }

    auto textureRefs() {
        return UIRectProperties::textureRefs();
    }

    auto textureRefs() const {
        return UIRectProperties::textureRefs();
    }

    okay::MaterialFlagCollection flags() {
        okay::MaterialFlagCollection flags = SceneMaterialProperties::flags();
        flags.addFlag(okay::MaterialFlags::UNLIT);
        return flags;
    }
};

}  // namespace dash

#endif  // __SPEEDOMETER_H__

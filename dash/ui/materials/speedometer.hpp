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
    okay::UniformProperty<glm::vec4, okay::FixedString("u_needleColor")> needleColor{
        glm::vec4(1.0f)};
    okay::UniformProperty<glm::vec4, okay::FixedString("u_bgColor")> bgColor{glm::vec4(1.0f)};

    auto uniformRefs() {
        return std::tuple_cat(okay::UIRectProperties::uniformRefs(),
            std::tie(angle, trailRads, trailColor, needleColor, bgColor));
    }

    auto uniformRefs() const {
        return std::tuple_cat(okay::UIRectProperties::uniformRefs(),
            std::tie(angle, trailRads, trailColor, needleColor, bgColor));
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

#endif  // __SPEEDOMETER_H__

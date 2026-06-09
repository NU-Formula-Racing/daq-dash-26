#ifndef __LAUNCH_CONTROL_MATERIAL_H__
#define __LAUNCH_CONTROL_MATERIAL_H__

#include <okay/okay.hpp>

#include <tuple>

namespace dash {

struct LaunchControlMaterial : public okay::UIRectProperties,
                               okay::OkayMaterialProperties<LaunchControlMaterial> {
   public:
    // Smaller radius = longer spikes reaching farther toward the center
    okay::UniformProperty<float, okay::FixedString("u_speedLineRadius")> speedLineRadius{0.025f};
    okay::UniformProperty<float, okay::FixedString("u_speedLineScale")> speedLineScale{50.0f};
    okay::UniformProperty<float, okay::FixedString("u_speedLineSpeed")> speedLineSpeed{2.0f};
    okay::UniformProperty<float, okay::FixedString("u_speedLineEdge")> speedLineEdge{0.25f};
    okay::UniformProperty<float, okay::FixedString("u_speedLineThickness")> speedLineThickness{
        2.0f};

    okay::UniformProperty<glm::vec4, okay::FixedString("u_vignetteInnerColor")> vignetteInnerColor{
        glm::vec4(1.0f, 0.9f, 0.2f, 1.0f)};
    okay::UniformProperty<glm::vec4, okay::FixedString("u_vignetteOuterColor")> vignetteOuterColor{
        glm::vec4(1.0f, 0.2f, 0.0f, 1.0f)};

    okay::UniformProperty<float, okay::FixedString("u_bgAlpha")> bgAlpha{0.5f};
    okay::UniformProperty<float, okay::FixedString("u_lineAlpha")> lineAlpha{2.0f};
    okay::UniformProperty<float, okay::FixedString("u_vignettePower")> vignettePower{1.5f};

    auto uniformRefs() {
        return std::tuple_cat(okay::UIRectProperties::uniformRefs(),
            std::tie(speedLineRadius,
                speedLineScale,
                speedLineSpeed,
                speedLineEdge,
                speedLineThickness,
                vignetteInnerColor,
                vignetteOuterColor,
                bgAlpha,
                lineAlpha,
                vignettePower));
    }

    auto uniformRefs() const {
        return std::tuple_cat(okay::UIRectProperties::uniformRefs(),
            std::tie(speedLineRadius,
                speedLineScale,
                speedLineSpeed,
                speedLineEdge,
                speedLineThickness,
                vignetteInnerColor,
                vignetteOuterColor,
                bgAlpha,
                lineAlpha,
                vignettePower));
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
        okay::MaterialFlagCollection flags = okay::UIRectProperties::flags();
        flags.addFlag(okay::MaterialFlags::TRANSPARENT);
        flags.addFlag(okay::MaterialFlags::UNLIT);
        return flags;
    }
};

}  // namespace dash

#endif  // __LAUNCH_CONTROL_MATERIAL_H__

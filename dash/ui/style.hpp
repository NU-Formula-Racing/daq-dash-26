#ifndef __STYLE_H__
#define __STYLE_H__

#include <okay/okay.hpp>

#include <glm/glm.hpp>

namespace dash {

namespace colors {

static inline constexpr glm::vec4 fromHex(const std::uint32_t color) {
    const float inv = 1.0f / 255.0f;

    return glm::vec4{
        static_cast<float>((color >> 24) & 0xFF) * inv,
        static_cast<float>((color >> 16) & 0xFF) * inv,
        static_cast<float>((color >> 8) & 0xFF) * inv,
        static_cast<float>((color >> 0) & 0xFF) * inv,
    };
}

static constexpr glm::vec4 black = glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
static constexpr glm::vec4 white = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
static constexpr glm::vec4 northwesternPurple = fromHex(0x4E2A84FF);

}  // namespace colors

namespace fonts {

okay::GameAssetRef<okay::FontManager::FontHandle, okay::FontLoadOptions> latoBlack{
    "fonts/Lato-Black.ttf"};
okay::GameAssetRef<okay::FontManager::FontHandle, okay::FontLoadOptions> latoBold{
    "fonts/Lato-Bold.ttf"};

};  // namespace fonts

}  // namespace dash

#endif

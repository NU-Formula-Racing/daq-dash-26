#include <platform/interfaces.hpp>

namespace dash {

struct NeopixelStrip::NeopixelImpl {};

NeopixelStrip::NeopixelStrip() : _impl(std::make_unique<NeopixelStrip::NeopixelImpl>()) {}

NeopixelStrip::~NeopixelStrip() {}

void NeopixelStrip::init(const int& pin, const int& numLeds) {
    // noop
}

void NeopixelStrip::setColor(const int& ledIndex, const glm::vec4& color) {
    // noop
}

void NeopixelStrip::show() {
    // noop
}

void NeopixelStrip::cleanup() {
    // noop
}

};  // namespace dash
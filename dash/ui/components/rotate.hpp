#ifndef __ROTATE_H__
#define __ROTATE_H__

#include <okay/okay.hpp>

#include <glm/glm.hpp>

namespace dash {

struct RotateComponent {
    float speed{5.0f};

    RotateComponent() {}
    RotateComponent(float speed) : speed(speed) {}
};

class RotateSystem
    : public okay::ECSSystem<okay::query::Get<okay::TransformComponent, RotateComponent>> {
   public:
    void onPreTick(QueryT::Item& item) override {
        auto& [transform, rotateComponent] = item.components;
        float theta =
            okay::Engine.time->timeSinceStartSec() * rotateComponent.speed * glm::pi<float>();
        glm::quat rot = glm::angleAxis(theta, glm::vec3(0.0, 1.0, 0.0f));
        transform->rotation = rot;
    }
};

}  // namespace dash

#endif  // __ROTATE_H__

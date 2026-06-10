#ifndef __ROTATE_H__
#define __ROTATE_H__

#include <okay/okay.hpp>

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>

namespace dash {

struct RotateComponent {
    float speed{5.0f};
    float minTheta{0.0f};
    float maxTheta{glm::pi<float>()};

    RotateComponent() {}
    RotateComponent(float speed) : speed(speed) {}
    RotateComponent(float speed, float minTheta, float maxTheta)
        : speed(speed), minTheta(minTheta), maxTheta(maxTheta) {}
};

class RotateSystem
    : public okay::ECSSystem<okay::query::Get<okay::TransformComponent, RotateComponent>> {
   public:
    void onPreTick(QueryT::Item& item) override {
        auto& [transform, rotateComponent] = item.components;

        float minTheta = rotateComponent.minTheta;
        float maxTheta = rotateComponent.maxTheta;

        if (maxTheta < minTheta) {
            std::swap(minTheta, maxTheta);
        }

        float range = maxTheta - minTheta;

        if (range <= 0.0f) {
            transform->rotation = glm::angleAxis(minTheta, glm::vec3(0.0f, 1.0f, 0.0f));
            return;
        }

        float t = okay::Engine.time->timeSinceStartSec();

        float sinValue = glm::sin(t * rotateComponent.speed);
        float normalized = (sinValue + 1.0f) * 0.5f;

        float theta = glm::mix(minTheta, maxTheta, normalized);

        transform->rotation = glm::angleAxis(theta, glm::vec3(0.0f, 1.0f, 0.0f));
    }
};

}  // namespace dash

#endif  // __ROTATE_H__

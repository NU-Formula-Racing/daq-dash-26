#include "glfw_input.hpp"
#include <GLFW/glfw3.h>
#include <cstring>

namespace dash {

bool GlfwInput::prevKeys[349] = { false };
bool GlfwInput::currKeys[349] = { false };

void GlfwInput::update(GLFWwindow* window) {
    if (window == nullptr) return;

    std::memcpy(prevKeys, currKeys, sizeof(currKeys));

    for (int i {32}; i <= GLFW_KEY_LAST; ++i) {
        currKeys[i] = glfwGetKey(window, i) == GLFW_PRESS;
    }
};

bool GlfwInput::isKeyDown(int key) {
    return currKeys[key];
}

bool GlfwInput::isKeyDownThisFrame(int key) {
    return !prevKeys[key] && currKeys[key];
}

bool GlfwInput::isKeyUpThisFrame(int key) {
    return prevKeys[key] && !currKeys[key];
}

}  // namespace dash

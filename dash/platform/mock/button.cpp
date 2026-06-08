#include <okay/okay.hpp>

#include <GLFW/glfw3.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <platform/button.hpp>
#include <unordered_map>

namespace dash {

static std::unordered_map<int, int> s_gpioKeyMap = {
    {21, GLFW_KEY_W}, {16, GLFW_KEY_A}, {20, GLFW_KEY_S}, {12, GLFW_KEY_D}, {23, GLFW_KEY_SPACE}};

struct ButtonContext {
    bool isDown = false;
    bool wasDown = false;

    std::vector<std::function<void()>> onDownList;
    std::vector<std::function<void()>> onUpList;

    void onDown() {
        for (auto cb : onDownList)
            cb();
    }

    void onUp() {
        for (auto cb : onUpList)
            cb();
    }
};

static std::unordered_map<int, ButtonContext> s_buttonContexts;
static GLFWkeyfun s_previousKeyCallback = nullptr;

static GLFWwindow* getWindow() {
    return (GLFWwindow*)okay::Engine.systems.getSystemChecked<okay::Renderer>()->getSurfaceWindow();
}

static void buttonKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Preserve any callback that was installed before us, like ImGui's.
    if (s_previousKeyCallback) {
        s_previousKeyCallback(window, key, scancode, action, mods);
    }

    for (auto& [gpioPin, glfwKey] : s_gpioKeyMap) {
        if (glfwKey != key) {
            continue;
        }

        ButtonContext& context = s_buttonContexts[gpioPin];

        if (action == GLFW_PRESS) {
            context.wasDown = context.isDown;
            context.isDown = true;

            context.onDown();
        } else if (action == GLFW_RELEASE) {
            context.wasDown = context.isDown;
            context.isDown = false;
            context.onUp();
        }

        break;
    }
}

static void ensureButtonKeyCallbackInstalled() {
    static bool installed = false;
    if (!installed) {
        GLFWwindow* window = getWindow();
        s_previousKeyCallback = glfwSetKeyCallback(window, buttonKeyCallback);
        installed = true;
    }
}

Button::Button(uint8_t gpioPin)
    : _buttonID(gpioPin), _gpio(std::make_unique<GPIO>(gpioPin, false)) {}

Button::~Button() = default;

void Button::onDown(std::function<void()> callback) {
    ensureButtonKeyCallbackInstalled();
    s_buttonContexts[_buttonID].onDownList.push_back(std::move(callback));
}

void Button::onUp(std::function<void()> callback) {
    ensureButtonKeyCallbackInstalled();
    s_buttonContexts[_buttonID].onUpList.push_back(std::move(callback));
}

bool Button::isDownThisFrame() {
    ButtonContext& state = s_buttonContexts[_buttonID];
    return state.isDown && !state.wasDown;
}

bool Button::isUpThisFrame() {
    ButtonContext& state = s_buttonContexts[_buttonID];
    return !state.isDown && state.wasDown;
}

bool Button::isDown() {
    int res = glfwGetKey(getWindow(), s_gpioKeyMap[_buttonID]);
    return res == GLFW_PRESS;
}

};  // namespace dash

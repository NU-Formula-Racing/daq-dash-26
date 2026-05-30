#ifndef __GLFW_INPUT_H__
#define __GLFW_INPUT_H__

#include <GLFW/glfw3.h>

namespace dash {

class GlfwInput {
   private:
    static bool prevKeys[GLFW_KEY_LAST + 1];
    static bool currKeys[GLFW_KEY_LAST + 1];
   public:
    static void update(GLFWwindow* window);

    static bool isKeyDown(int key);

    static bool isKeyDownThisFrame(int key);
    static bool isKeyUpThisFrame(int key);
};

}  // namespace dash

#endif  // __GLFW_INPUT_H__

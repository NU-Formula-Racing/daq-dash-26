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
    static bool getKey(int key);
    static bool getKeyDown(int key);
    static bool getKeyUp(int key);
};

}  // namespace dash

#endif  // __GLFW_INPUT_H__

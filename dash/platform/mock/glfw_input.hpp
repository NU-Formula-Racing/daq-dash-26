#ifndef __GLFW_INPUT_H__
#define __GLFW_INPUT_H__

#include <GLFW/glfw3.h>
#include <cstring>

namespace dash {

class GlfwInput {
   private:
    static inline bool prevKeys[GLFW_KEY_LAST + 1] = { false };
    static inline bool currKeys[GLFW_KEY_LAST + 1] = { false };
   public:
    static inline void update(GLFWwindow* window) {
        std::memcpy(prevKeys, currKeys, sizeof(currKeys));

        for (int i {32}; i <= GLFW_KEY_LAST; ++i) {
            currKeys[i] = glfwGetKey(window, i) == GLFW_PRESS;
        }
    };
    
    static inline bool getKey(int key) {
        return currKeys[key];
    }

    static inline bool getKeyDown(int key) {
        return !prevKeys[key] && currKeys[key];
    }

    static inline bool getKeyUp(int key) {
        return prevKeys[key] && !currKeys[key];
    }
};

}  // namespace dash

#endif  // __GLFW_INPUT_H__

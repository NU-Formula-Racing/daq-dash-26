#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <functional>
#include <memory>
#include <platform/interfaces.hpp>
#include <stdint.h>

namespace dash {

class Button {
   public:
    enum class ButtonState { DOWN_THIS_FRAME, DOWN, UP_THIS_FRAME, UP };

    Button(uint8_t gpioPin);
    ~Button();

    void onDown(std::function<void()> callback);
    void onUp(std::function<void()> callback);

    bool isDownThisFrame();
    bool isUpThisFrame();
    bool isDown();

   private:
    uint8_t _buttonID;
    std::unique_ptr<GPIO> _gpio;
    std::uint32_t _lastDown{0};
    std::uint32_t _lastUp{0};
};

}  // namespace dash

#endif  // __BUTTON_H__

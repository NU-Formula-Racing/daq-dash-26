#ifndef __ENCODER_H__
#define __ENCODER_H__

#include <functional>
#include <memory>
#include <platform/interfaces.hpp>

namespace dash {

// figure out how to implement this
class Encoder {
   public:
    enum class EncoderState { LEFT_THIS_FRAME, RIGHT_THIS_FRAME, IDLE };

    Encoder(uint8_t leftPin, uint8_t rightPin);
    ~Encoder();

    void onRight(std::function<void()> callback);
    void onLeft(std::function<void()> callback);

    bool isIdle();
    bool isRightThisFrame();
    bool isLeftThisFrame();

   private:
    constexpr uint16_t generateID(uint8_t left, uint8_t right);
    uint16_t _encoderID;
    std::unique_ptr<GPIO> _leftGPIO;
    std::unique_ptr<GPIO> _rightGPIO;
};

};  // namespace dash

#endif  // __ENCODER_H__

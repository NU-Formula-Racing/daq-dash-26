#ifndef __INPUT_MANAGER_H__
#define __INPUT_MANAGER_H__

#include <cstdint>
#include <functional>
#include <platform/button.hpp>
#include <platform/encoder.hpp>
#include <unordered_map>
#include <vector>

namespace dash {

class InputManager {
   public:
    static InputManager& instance();

    void registerButton(uint8_t buttonID);
    void unregisterButton(uint8_t buttonID);

    void attachDownCallback(uint8_t buttonID, std::function<void()> callback);
    void attachUpCallback(uint8_t buttonID, std::function<void()> callback);

    void executeDownCallbacks(uint8_t buttonID);
    void executeUpCallbacks(uint8_t buttonID);

    void registerEncoder(uint16_t encoderID, uint8_t leftPin, uint8_t rightPin);
    void unregisterEncoder(uint16_t encoderID);

    void attachLeftCallback(uint16_t encoderID, std::function<void()> callback);
    void attachRightCallback(uint16_t encoderID, std::function<void()> callback);

    void executeLeftCallbacks(uint16_t encoderID);
    void executeRightCallbacks(uint16_t encoderID);

    void onEncoderEdge(uint16_t encoderID);

    // add left and right callbacks, which take the left or right pin number

    bool isDownThisFrame(uint8_t buttonID);
    bool isUpThisFrame(uint8_t buttonID);
    bool isDown(uint8_t buttonID);

    bool isRightThisFrame(uint16_t encoderID) const;
    bool isLeftThisFrame(uint16_t encoderID) const;
    bool isIdle(uint16_t encoderID) const;

    void tick();

   private:
    InputManager();
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    std::unordered_map<uint8_t, bool> _buttonEventHappened;
    std::unordered_map<uint8_t, std::vector<std::function<void()>>> _downCallbacks;
    std::unordered_map<uint8_t, std::vector<std::function<void()>>> _upCallbacks;
    std::unordered_map<uint8_t, Button::ButtonState> _buttonStates;

    struct EncoderRuntime {
        uint8_t pinA = 0;
        uint8_t pinB = 0;
        uint8_t prevAB = 0;
        int8_t accum = 0;
        bool initialized = false;
        Encoder::EncoderState state = Encoder::EncoderState::IDLE;
    };

    std::unordered_map<uint16_t, std::vector<std::function<void()>>> _leftCallbacks;
    std::unordered_map<uint16_t, std::vector<std::function<void()>>> _rightCallbacks;
    std::unordered_map<uint16_t, EncoderRuntime> _encoderStates;
};

};  // namespace dash

#endif  // __INPUT_MANAGER_H__
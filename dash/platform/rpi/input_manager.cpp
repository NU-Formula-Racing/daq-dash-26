#include <platform/platform.hpp>
#include <platform/rpi/gpio_manager.hpp>

namespace dash::platform {

InputManager::InputManager() = default;

InputManager& InputManager::instance(){
    static InputManager instance;
    return instance;
}

void InputManager::registerButton(uint8_t buttonID){
    _buttonStates[buttonID] = Button::ButtonState::UP;
}

void InputManager::unregisterButton(uint8_t buttonID){
    _downCallbacks.erase(buttonID);
    _buttonStates.erase(buttonID);
    _upCallbacks.erase(buttonID);
    _buttonEventHappened.erase(buttonID);
}

void InputManager::attachDownCallback(uint8_t buttonID, std::function<void()> callback){
    _downCallbacks[buttonID].push_back(std::move(callback));
}

void InputManager::attachUpCallback(uint8_t buttonID, std::function<void()> callback){
    _upCallbacks[buttonID].push_back(std::move(callback));
}

void InputManager::executeDownCallbacks(uint8_t buttonID){
    for(auto& callback : _downCallbacks[buttonID]){
        callback();
    }
    _buttonEventHappened[buttonID] = true;
}

void InputManager::executeUpCallbacks(uint8_t buttonID){
    for(auto& callback : _upCallbacks[buttonID]){
        callback();
    }
    _buttonEventHappened[buttonID] = true;
}

bool InputManager::isDownThisFrame(uint8_t buttonID){
    return _buttonStates[buttonID] == Button::ButtonState::DOWN_THIS_FRAME;
}

bool InputManager::isUpThisFrame(uint8_t buttonID){
    return _buttonStates[buttonID] == Button::ButtonState::UP_THIS_FRAME;
}

bool InputManager::isDown(uint8_t buttonID){
    return _buttonStates[buttonID] == Button::ButtonState::DOWN;
}

void InputManager::registerEncoder(uint16_t encoderID, uint8_t pinA, uint8_t pinB){
    _encoderStates[encoderID] = EncoderRuntime{.pinA = pinA, .pinB = pinB};
}

void InputManager::unregisterEncoder(uint16_t encoderID){
    _leftCallbacks.erase(encoderID);
    _rightCallbacks.erase(encoderID);
    _encoderStates.erase(encoderID);
}

void InputManager::attachLeftCallback(uint16_t encoderID, std::function<void()> callback) {
  _leftCallbacks[encoderID].push_back(std::move(callback));
}

void InputManager::attachRightCallback(uint16_t encoderID, std::function<void()> callback) {
  _rightCallbacks[encoderID].push_back(std::move(callback));
}

bool InputManager::isLeftThisFrame(uint16_t encoderID) const {
  auto it = _encoderStates.find(encoderID);
  return it != _encoderStates.end() && it->second.state == Encoder::EncoderState::LEFT_THIS_FRAME;
}

bool InputManager::isRightThisFrame(uint16_t encoderID) const {
  auto it = _encoderStates.find(encoderID);
  return it != _encoderStates.end() && it->second.state == Encoder::EncoderState::RIGHT_THIS_FRAME;
}

bool InputManager::isIdle(uint16_t encoderID) const {
    auto it = _encoderStates.find(encoderID);
  return it != _encoderStates.end() && it->second.state == Encoder::EncoderState::IDLE;
}

void InputManager::onEncoderEdge(uint16_t encoderID) {
  auto it = _encoderStates.find(encoderID);
  if (it == _encoderStates.end()) return;

  auto& e = it->second;

  GpioLevel a = GpioLevel::G_LOW;
  GpioLevel b = GpioLevel::G_LOW;
  GPIOManager::instance().gpioReadPin(e.pinA, a);
  GPIOManager::instance().gpioReadPin(e.pinB, b);

  const uint8_t currAB =
      (static_cast<uint8_t>(a == GpioLevel::G_HIGH) << 1) |
      static_cast<uint8_t>(b == GpioLevel::G_HIGH);

  if (!e.initialized) {
    e.prevAB = currAB;
    e.initialized = true;
    return;
  }

  static constexpr int8_t kQuad[16] = {
      0, -1, +1,  0,
      +1, 0,  0, -1,
      -1, 0,  0, +1,
      0, +1, -1,  0
  };

  const uint8_t idx = static_cast<uint8_t>((e.prevAB << 2) | currAB);
  e.accum += kQuad[idx];
  e.prevAB = currAB;

  if (e.accum >= 4) {
    e.state = Encoder::EncoderState::RIGHT_THIS_FRAME;
    for (auto& cb : _rightCallbacks[encoderID]){
        cb();
    } 

    e.accum = 0;

  } else if (e.accum <= -4) {
    e.state = Encoder::EncoderState::LEFT_THIS_FRAME;
    for (auto& cb : _leftCallbacks[encoderID]){
        cb();
    } 

    e.accum = 0;

  }
}

void InputManager::tick(){
    for(auto& [buttonID, eventIndicator] : _buttonEventHappened){
        if (eventIndicator && _buttonStates[buttonID] == Button::ButtonState::UP) {
            _buttonStates[buttonID] = Button::ButtonState::DOWN_THIS_FRAME;
        } else if (eventIndicator && _buttonStates[buttonID] == Button::ButtonState::DOWN) {
            _buttonStates[buttonID] = Button::ButtonState::UP_THIS_FRAME;
        } else if (_buttonStates[buttonID] == Button::ButtonState::DOWN_THIS_FRAME) {
            _buttonStates[buttonID] = Button::ButtonState::DOWN;
        } else if (_buttonStates[buttonID] == Button::ButtonState::UP_THIS_FRAME) {
            _buttonStates[buttonID] = Button::ButtonState::UP;
        }

        _buttonEventHappened[buttonID] = false;
    }

    for (auto& [encoderID, encoder] : _encoderStates) {
        if (encoder.state == Encoder::EncoderState::LEFT_THIS_FRAME ||
            encoder.state == Encoder::EncoderState::RIGHT_THIS_FRAME) {
            encoder.state = Encoder::EncoderState::IDLE;
        }
    }
}

}

#include <platform/platform.hpp>

namespace dash::platform {

constexpr uint16_t Encoder::generateID(uint8_t leftPin, uint8_t rightPin){
    return (leftPin << 8) | (rightPin << 0);
}

Encoder::Encoder(uint8_t leftPin, uint8_t rightPin) :
    _encoderID(generateID(leftPin, rightPin)),
    _leftGPIO(std::make_unique<GPIO>(leftPin, false)),
    _rightGPIO(std::make_unique<GPIO>(rightPin, false))
{
    InputManager::instance().registerEncoder(_encoderID, leftPin, rightPin);

    _leftGPIO->attachInterrupt([id = _encoderID](){
        InputManager::instance().onEncoderEdge(id);
    }, GPIO::EdgeType::BOTH);

    _rightGPIO->attachInterrupt([id = _encoderID](){
        InputManager::instance().onEncoderEdge(id);
    }, GPIO::EdgeType::BOTH);

}

Encoder::~Encoder(){
    InputManager::instance().unregisterEncoder(_encoderID);
}

void Encoder::onRight(std::function<void()> callback){
    InputManager::instance().attachRightCallback(_encoderID, std::move(callback));
}

void Encoder::onLeft(std::function<void()> callback){
    InputManager::instance().attachLeftCallback(_encoderID, std::move(callback));
}

bool Encoder::isIdle(){
    return InputManager::instance().isIdle(_encoderID);
}

bool Encoder::isRightThisFrame(){
    return InputManager::instance().isRightThisFrame(_encoderID);
}

bool Encoder::isLeftThisFrame(){
    return InputManager::instance().isLeftThisFrame(_encoderID);
}

}

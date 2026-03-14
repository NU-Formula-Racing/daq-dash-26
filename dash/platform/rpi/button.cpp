#include <platform/platform.hpp>

namespace dash::platform {

Button::Button(uint8_t gpioPin) : 
    _buttonID(gpioPin), 
    _gpio(std::make_unique<GPIO>(gpioPin, false))
{
    InputManager::instance().registerButton(_buttonID);

    _gpio->attachInterrupt([id = _buttonID](){
        InputManager::instance().executeUpCallbacks(id);
    }, GPIO::EdgeType::FALLING);

    _gpio->attachInterrupt([id = _buttonID](){
        InputManager::instance().executeDownCallbacks(id);
    }, GPIO::EdgeType::RISING);
}

Button::~Button(){
    InputManager::instance().unregisterButton(_buttonID);
}

void Button::onDown(std::function<void()> callback){
    InputManager::instance().attachDownCallback(_buttonID, std::move(callback));
}

void Button::onUp(std::function<void()> callback){
    InputManager::instance().attachUpCallback(_buttonID, std::move(callback));
}

bool Button::isDownThisFrame(){
    return InputManager::instance().isDownThisFrame(_buttonID);
}

bool Button::isUpThisFrame(){
    return InputManager::instance().isUpThisFrame(_buttonID);
}

bool Button::isDown(){
    return InputManager::instance().isDown(_buttonID);
}

}

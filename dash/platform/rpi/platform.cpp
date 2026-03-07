#include <platform/platform.hpp>
#include <platform/rpi/gpio_manager.hpp>

namespace dash::platform {

void tick(){
    GPIOManager::instance().tick();
    InputManager::instance().tick();
}

}

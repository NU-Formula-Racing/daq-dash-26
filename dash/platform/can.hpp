#ifndef __CAN_H__
#define __CAN_H__

#include <okay/okay.hpp>

#include <mutex>
#include <thread>

namespace dash {

class CANManager : public okay::System<okay::SystemScope::GAME> {
   public:
    virtual void initialize();
    virtual void tick();

    std::mutex busLock;
};

};  // namespace dash

#endif  // __CAN_H__

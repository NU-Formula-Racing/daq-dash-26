#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <okay/okay.hpp>
#include <nfr_can/CAN_interface.hpp>

namespace dash {

class Platform : public okay::System<okay::SystemScope::ENGINE> {
   public:
    virtual void initialize();
    virtual void tick();

    void configureCANDriver(CAN_Bus& bus);
};

};  // namespace dash

#endif  // __PLATFORM_H__
#ifndef __INPUTS_H__
#define __INPUTS_H__

#include <platform/button.hpp>
#include <platform/encoder.hpp>

namespace dash {

namespace input {

inline dash::Button upButton{21};
inline dash::Button downButton{20};
inline dash::Button leftButton{16};
inline dash::Button rightButton{12};
inline dash::Button centerButton{23};

inline dash::Encoder encoder{6, 5};
inline dash::Button encoderButton{24};

};  // namespace input

};  // namespace dash

#endif  // __INPUTS_H__

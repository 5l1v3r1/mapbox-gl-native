#include <mbgl/text/placement_types.hpp>

#include <cmath>

namespace mbgl {

OpacityState::OpacityState(bool placed_, bool skipFade) : opacity((skipFade && placed_) ? 1 : 0), placed(placed_) {}

OpacityState::OpacityState(const OpacityState& prevState, float increment, bool placed_)
    : opacity(::fmax(0, ::fmin(1, prevState.opacity + (prevState.placed ? increment : -increment)))), placed(placed_) {}

bool OpacityState::isHidden() const {
    return opacity == 0 && !placed;
}

JointOpacityState::JointOpacityState(bool placedText, bool placedIcon, bool skipFade)
    : icon(OpacityState(placedIcon, skipFade)), text(OpacityState(placedText, skipFade)) {}

JointOpacityState::JointOpacityState(const JointOpacityState& prevOpacityState,
                                     float increment,
                                     bool placedText,
                                     bool placedIcon)
    : icon(OpacityState(prevOpacityState.icon, increment, placedIcon)),
      text(OpacityState(prevOpacityState.text, increment, placedText)) {}

bool JointOpacityState::isHidden() const {
    return icon.isHidden() && text.isHidden();
}

} // namespace mbgl
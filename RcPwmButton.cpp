#include "RcPwmButton.h"
#include "AppConstants.h"

RcPwmButton::RcPwmButton(int id) : ButtonBase(id) {}

void RcPwmButton::init(RCChannel* rcChannel) {
  _rcChannel = rcChannel;
}

bool RcPwmButton::getIsPressed() {
  bool isPressed = _rcChannel->isInMax();

  return isPressed;
}

#include "RcPwmButton.h"

RcPwmButton::RcPwmButton(int id)
  : ButtonBase(id) {}

void RcPwmButton::init(RCChannel* rcChannel) {
  _rcChannel = rcChannel;
  _rcChannel->setTolerance(200);
}

bool RcPwmButton::getIsPressed() {
  bool isPressed = _rcChannel->isInMax();

  return isPressed;
}

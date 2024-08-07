#pragma once

#include <Arduino.h>
#include "ButtonBase.h"
#include "RCChannel.h"

class RcPwmButton : public ButtonBase {
public:
  RcPwmButton(int id);

  void init(RCChannel* rcChannel);

private:
  RCChannel* _rcChannel;

protected:
  bool getIsPressed() override;
};

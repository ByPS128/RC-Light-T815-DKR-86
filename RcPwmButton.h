#pragma once

#include <Arduino.h>
#include "ButtonBase.h"

class RcPwmButton : public ButtonBase {
public:
  RcPwmButton(int id);

  void init(byte buttonPin) override;
  bool hasValidSignal() const;

protected:
  bool readPinState() override;

private:
  bool _hasValidSignal;
};

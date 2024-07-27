#pragma once

#include "ButtonBase.h"

class DigitalPullUpButton : public ButtonBase {
public:
  DigitalPullUpButton(int id);
  
  void init(byte buttonPin) override;

protected:
  bool readPinState() override;
};
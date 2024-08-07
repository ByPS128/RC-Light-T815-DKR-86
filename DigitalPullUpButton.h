#pragma once

#include "ButtonBase.h"

class DigitalPullUpButton : public ButtonBase {
public:
  DigitalPullUpButton(int id);

  void init(byte buttonPin);

protected:
  bool getIsPressed() override;

private:
  byte _pin;
};
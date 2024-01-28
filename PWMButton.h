#pragma once

#include <Arduino.h>

enum class ButtonClickKind {
  Click,
  DoubleClick,
  LongPress,
  ClickAndLongPress
};

class IButtonPressListener {
public:
  virtual void onClick(ButtonClickKind clickKind) = 0;
};

class PWMButton {
public:
  PWMButton();

  void init(byte pwmPin, IButtonPressListener* listener);
  void update();

private:
  uint8_t pwmPin;
  IButtonPressListener* listener;

  unsigned long lastPressTime;
  unsigned long lastReleaseTime;
  unsigned long pressDuration;
  unsigned long singleClickStartTime;

  const int LONG_PRESS_DURATION = 500;
  const int DOUBLE_CLICK_TIMEOUT = 250;

  bool isPressed;
  bool isLongPressed;
  int pressCount;

  void callListener();
  ButtonClickKind determineEventType();
};

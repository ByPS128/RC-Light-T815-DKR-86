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
  bool update();

private:
  uint8_t pwmPin;
  IButtonPressListener* listener;

  unsigned long lastPressTime;
  unsigned long lastReleaseTime;
  unsigned long pressDuration;
  unsigned long singleClickStartTime;

  const unsigned long LONG_PRESS_DURATION = 500;
  const unsigned long DOUBLE_CLICK_TIMEOUT = 250;

  bool isPressed;
  bool isLongPressed;
  int pressCount;
  bool hasValidSignal;

  void callListener();
  ButtonClickKind determineEventType();
};

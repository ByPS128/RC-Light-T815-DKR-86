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
  bool hasValidSignal();

private:
  const unsigned long LONG_PRESS_DURATION = 500;
  const unsigned long DOUBLE_CLICK_TIMEOUT = 250;
  
private:
  byte _pwmPin;
  IButtonPressListener* _listener;

  unsigned long _lastPressTime;
  unsigned long _lastReleaseTime;
  unsigned long _pressDuration;
  unsigned long _singleClickStartTime;

  bool _isPressed;
  bool _isLongPressed;
  int _pressCount;
  bool _hasValidSignal;

  void callListener();
  ButtonClickKind determineEventType();
};

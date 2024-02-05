#pragma once

#include <Arduino.h>
#include <LinkedList.h>

// Supported button press types
enum class RCButtonClickKind {
  Click,
  DoubleClick,
  LongPress,
  ClickAndLongPress
};

// A descendant implementing this abstract class (interface) can be registered as a subscriber of button events.
class IRCButtonClickSubscriber {
public:
  virtual void onButtonClick(RCButtonClickKind clickKind) = 0;
};

class RCButtonClickHandler {
public:
  RCButtonClickHandler();

  void init(byte pwmPin);
  void registerSubscriber(IRCButtonClickSubscriber* subscriber);
  void unregisterSubscriber(IRCButtonClickSubscriber* subscriber);
  bool update();
  bool hasValidSignal();

private:
  const unsigned long LONG_PRESS_DURATION = 500;
  const unsigned long DOUBLE_CLICK_TIMEOUT = 250;

private:
  LinkedList<IRCButtonClickSubscriber*> _subscribers;
  byte _pwmPin;

  unsigned long _lastPressTime;
  unsigned long _lastReleaseTime;
  unsigned long _pressDuration;
  unsigned long _singleClickStartTime;

  bool _isPressed;
  bool _isLongPressed;
  int _pressCount;
  bool _hasValidSignal;

  void notifySubscribers();
  RCButtonClickKind determineEventType();
};

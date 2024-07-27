#pragma once

#include <Arduino.h>
#include <LinkedList.h>

// Supported button press types
enum class ButtonClickType {
  Click,
  DoubleClick,
  LongPress,
  ClickAndLongPress
};

// A descendant implementing this abstract class (interface) can be registered as a subscriber of button events.
class IButtonEventSubscriber {
public:
  virtual void onButtonClick(int buttonId, ButtonClickType clickKind) = 0;
};

class RcPwmButton {
public:
  RcPwmButton(int id);

  void init(byte buttonPin);
  void registerSubscriber(IButtonEventSubscriber* subscriber);
  void unregisterSubscriber(IButtonEventSubscriber* subscriber);
  bool update();
  bool hasValidSignal();

private:
  const unsigned long LONG_PRESS_DURATION = 500;
  const unsigned long DOUBLE_CLICK_TIMEOUT = 250;

private:
  LinkedList<IButtonEventSubscriber*> _subscribers;
  int _id;
  byte _pin;

  unsigned long _lastPressTime;
  unsigned long _lastReleaseTime;
  unsigned long _pressDuration;
  unsigned long _singleClickStartTime;

  bool _isPressed;
  bool _isLongPressed;
  int _pressCount;
  bool _hasValidSignal;

  void notifySubscribers();
  ButtonClickType determineEventType();
};

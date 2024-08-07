#include "ButtonBase.h"
#include "AppConstants.h"

ButtonBase::ButtonBase(int id)
  : _id(id),
    _lastPressTime(0),
    _lastReleaseTime(0),
    _pressDuration(0),
    _isPressed(false),
    _isLongPressed(false),
    _pressCount(0) {
}

void ButtonBase::registerSubscriber(IButtonEventSubscriber* subscriber) {
  if (subscriber) {
    _subscribers.add(subscriber);
  }
}

void ButtonBase::unregisterSubscriber(IButtonEventSubscriber* subscriber) {
  for (int i = 0; i < _subscribers.size(); i++) {
    if (_subscribers.get(i) == subscriber) {
      _subscribers.remove(i);
      break;
    }
  }
}

// Returns booleas in meaning of hasValidSignal?
void ButtonBase::update() {
  bool pressedCurrent = getIsPressed();

  unsigned long currentTime = millis();
  if (pressedCurrent && !_isPressed) {  // Button pressed
    _isPressed = true;
    _lastPressTime = currentTime;
    _pressCount++;
  } else if (!pressedCurrent && _isPressed) {  // Button released
    _isPressed = false;
    _lastReleaseTime = currentTime;
    _pressDuration = currentTime - _lastPressTime;

    if (_pressDuration >= LONG_PRESS_DURATION) {
      _isLongPressed = true;
    }

    if (_pressCount == 1) {
      // Start timer to detect single or double clicks
      _singleClickStartTime = currentTime;
    }
  }

  if ((!_isPressed && _pressCount == 1 && _singleClickStartTime != 0 && currentTime - _lastReleaseTime > DOUBLE_CLICK_TIMEOUT) || (!_isPressed && _pressCount > 1)) {
    // Time for double click has elapsed, process event
    notifySubscribers();
    _singleClickStartTime = 0;  // Reset timer
    _pressCount = 0;
    _isLongPressed = false;
  }
}

void ButtonBase::notifySubscribers() {
  ButtonClickType clickKind = determineEventType();
  for (int i = 0; i < _subscribers.size(); i++) {
    IButtonEventSubscriber* subscriber = _subscribers.get(i);
    if (subscriber) {
      subscriber->onButtonClick(_id, clickKind);
    }
  }
}

ButtonClickType ButtonBase::determineEventType() {
  if (_isLongPressed && _pressCount == 1) {
    return ButtonClickType::LongPress;
  } else if (_isLongPressed && _pressCount > 1) {
    return ButtonClickType::ClickAndLongPress;
  } else if (_pressCount > 1) {
    return ButtonClickType::DoubleClick;
  } else if (_isLongPressed) {
    return ButtonClickType::LongPress;
  } else {
    return ButtonClickType::Click;
  }
}

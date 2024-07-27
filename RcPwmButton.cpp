#include "RcPwmButton.h"
#include "AppConstants.h"

RcPwmButton::RcPwmButton(int id)
  : _id(id),
    _pin(-1), 
    _lastPressTime(0),
    _lastReleaseTime(0),
    _pressDuration(0),
    _isPressed(false),
    _isLongPressed(false),
    _pressCount(0),
    _hasValidSignal(false) {
}

void RcPwmButton::init(byte buttonPin) {
  _pin = buttonPin;
  pinMode(_pin, INPUT);
}

void RcPwmButton::registerSubscriber(IButtonEventSubscriber* subscriber) {
  if (subscriber) {
    _subscribers.add(subscriber);
  }
}

void RcPwmButton::unregisterSubscriber(IButtonEventSubscriber* subscriber) {
  for (int i = 0; i < _subscribers.size(); i++) {
    if (_subscribers.get(i) == subscriber) {
      _subscribers.remove(i);
      break;
    }
  }
}

// Returns booleas in meaning of hasValidSignal?
bool RcPwmButton::update() {
  unsigned long pwmValue = pulseIn(_pin, HIGH);
  _hasValidSignal = pwmValue >= SIGNAL_VALID_LOW_VALUE_THRESHOLD && pwmValue <= SIGNAL_VALID_HIGH_VALUE_THRESHOLD;
  // Serial.print("signal valid? (");
  // Serial.print(pwmValue);
  // Serial.print("): ");
  // Serial.println(_hasValidSignal);
  if (!_hasValidSignal) {
    return false;  // signal is invalid
  }

  int buttonValue = map(pwmValue, 1000, 2000, BYTE_MIN, BYTE_MAX);
  buttonValue = constrain(buttonValue, BYTE_MIN, BYTE_MAX);
  bool pressedCurrent = buttonValue > BYTE_MID;

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

  return true;  // signal is valid
}

bool RcPwmButton::hasValidSignal() {
  return _hasValidSignal;
}

void RcPwmButton::notifySubscribers() {
  ButtonClickType clickKind = determineEventType();
  for (int i = 0; i < _subscribers.size(); i++) {
    IButtonEventSubscriber* subscriber = _subscribers.get(i);
    if (subscriber) {
      subscriber->onButtonClick(_id, clickKind);
    }
  }
}

ButtonClickType RcPwmButton::determineEventType() {
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

#include "RCButtonClickHandler.h"
#include "AppConstants.h"

RCButtonClickHandler::RCButtonClickHandler()
  : _lastPressTime(0),
    _lastReleaseTime(0),
    _pressDuration(0),
    _isPressed(false),
    _isLongPressed(false),
    _pressCount(0),
    _hasValidSignal(false) {
  //
  pinMode(_pwmPin, INPUT);
}

void RCButtonClickHandler::init(byte pwmPin) {
  _pwmPin = pwmPin;
}

void RCButtonClickHandler::registerSubscriber(IRCButtonClickSubscriber* subscriber) {
  if (subscriber) {
    _subscribers.add(subscriber);
  }
}

void RCButtonClickHandler::unregisterSubscriber(IRCButtonClickSubscriber* subscriber) {
  for (int i = 0; i < _subscribers.size(); i++) {
    if (_subscribers.get(i) == subscriber) {
      _subscribers.remove(i);
      break;
    }
  }
}

// Returns booleas in meaning of hasValidSignal?
bool RCButtonClickHandler::update() {
  unsigned long pwmValue = pulseIn(_pwmPin, HIGH);
  _hasValidSignal = pwmValue >= SIGNAL_VALID_LOW_VALUE_THRESHOLD && pwmValue <= SIGNAL_VALID_HIGH_VALUE_THRESHOLD;
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

bool RCButtonClickHandler::hasValidSignal() {
  return _hasValidSignal;
}

void RCButtonClickHandler::notifySubscribers() {
  RCButtonClickKind clickKind = determineEventType();
  for (int i = 0; i < _subscribers.size(); i++) {
    IRCButtonClickSubscriber* subscriber = _subscribers.get(i);
    if (subscriber) {
      subscriber->onButtonClick(clickKind);
    }
  }
}

RCButtonClickKind RCButtonClickHandler::determineEventType() {
  if (_isLongPressed && _pressCount == 1) {
    return RCButtonClickKind::LongPress;
  } else if (_isLongPressed && _pressCount > 1) {
    return RCButtonClickKind::ClickAndLongPress;
  } else if (_pressCount > 1) {
    return RCButtonClickKind::DoubleClick;
  } else if (_isLongPressed) {
    return RCButtonClickKind::LongPress;
  } else {
    return RCButtonClickKind::Click;
  }
}

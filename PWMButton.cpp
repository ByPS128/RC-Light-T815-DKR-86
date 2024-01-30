#include "PWMButton.h"
#include "AppConstants.h"

PWMButton::PWMButton()
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

void PWMButton::init(IButtonPressListener* listener, byte pwmPin) {
  _listener = listener;
  _pwmPin = pwmPin;
}

// Returns booleas in meaning of hasValidSignal?
bool PWMButton::update() {
  unsigned long pwmValue = pulseIn(_pwmPin, HIGH);
  _hasValidSignal = pwmValue >= SIGNAL_VALID_LOW_VALUE_THRESHOLD && pwmValue <= SIGNAL_VALID_HIGH_VALUE_THRESHOLD;
  if (!_hasValidSignal) {
    return false;  // signal is invalid
  }

  int buttonValue = map(pwmValue, 1000, 2000, BYTE_MIN, BYTE_MAX);
  buttonValue = constrain(buttonValue, BYTE_MIN, BYTE_MAX);
  bool pressedCurrent = buttonValue > BYTE_MID;

  unsigned long currentTime = millis();
  if (pressedCurrent && !_isPressed) {  // Tlačítko bylo stisknuto
    _isPressed = true;
    _lastPressTime = currentTime;
    _pressCount++;
  } else if (!pressedCurrent && _isPressed) {  // Tlačítko bylo uvolněno
    _isPressed = false;
    _lastReleaseTime = currentTime;
    _pressDuration = currentTime - _lastPressTime;

    if (_pressDuration >= LONG_PRESS_DURATION) {
      _isLongPressed = true;
    }

    if (_pressCount == 1) {
      // Spustit časovač pro detekci jednoduchého nebo dvojitého kliknutí
      _singleClickStartTime = currentTime;
    }
  }

  if ((!_isPressed && _pressCount == 1 && _singleClickStartTime != 0 && currentTime - _lastReleaseTime > DOUBLE_CLICK_TIMEOUT) || (!_isPressed && _pressCount > 1)) {
    // Čas pro dvojité kliknutí uplynul, zpracovat událost
    callListener();
    _singleClickStartTime = 0;  // Reset časovače
    _pressCount = 0;
    _isLongPressed = false;
  }

  return true;  // signal is valid
}

bool PWMButton::hasValidSignal() {
  return _hasValidSignal;
}

void PWMButton::callListener() {
  if (_listener) {
    _listener->onClick(determineEventType());
  }
}

ButtonClickKind PWMButton::determineEventType() {
  if (_isLongPressed && _pressCount == 1) {
    return ButtonClickKind::LongPress;
  } else if (_isLongPressed && _pressCount > 1) {
    return ButtonClickKind::ClickAndLongPress;
  } else if (_pressCount > 1) {
    return ButtonClickKind::DoubleClick;
  } else if (_isLongPressed) {
    return ButtonClickKind::LongPress;
  } else {
    return ButtonClickKind::Click;
  }
}

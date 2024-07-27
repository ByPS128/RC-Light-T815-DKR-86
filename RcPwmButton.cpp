#include "RcPwmButton.h"
#include "AppConstants.h"

RcPwmButton::RcPwmButton(int id)
  : ButtonBase(id), _hasValidSignal(false) {

}

bool RcPwmButton::hasValidSignal() const {
  return _hasValidSignal;
}

// Returns booleas in meaning of hasValidSignal?
bool RcPwmButton::readPinState() {
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

  return pressedCurrent;
}

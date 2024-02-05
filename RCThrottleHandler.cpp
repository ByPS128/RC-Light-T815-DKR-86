#include <HardwareSerial.h>
#include "RCThrottleHandler.h"

RCThrottleHandler::RCThrottleHandler()
  : _hasValidSignal(false),
    _throttleChannelPwmPin(0),
    _analogMotorForwardPin(0),
    _analogMotorBackwardPin(0),
    _throttleReducedValue(0),
    _forwardReducedValue(0),
    _backwardReducedValue(0) {
  //
}

void RCThrottleHandler::init(byte throttleChannelPwmPin, byte analogMotorForwardPin, byte analogMotorBackwardPin) {
  _throttleChannelPwmPin = throttleChannelPwmPin;
  _analogMotorForwardPin = analogMotorForwardPin;
  _analogMotorBackwardPin = analogMotorBackwardPin;
}

// Returns booleas in meaning of hasValidSignal?
bool RCThrottleHandler::update() {
  unsigned long pwmRawValue = pulseIn(_throttleChannelPwmPin, HIGH);
  _hasValidSignal = pwmRawValue >= SIGNAL_VALID_LOW_VALUE_THRESHOLD && pwmRawValue <= SIGNAL_VALID_HIGH_VALUE_THRESHOLD;
  if (!_hasValidSignal) {
    return false;  // signal is invalid
  }

  // Range bounds 1000 and 2000 will be result of callibration, in future.
  int scaledDownPwmValue = map(pwmRawValue, 1000, 2000, BYTE_MIN, BYTE_MAX);
  _throttleReducedValue = constrain(scaledDownPwmValue, BYTE_MIN, BYTE_MAX);

  int forwadRawValue = analogRead(_analogMotorForwardPin);
  _forwardReducedValue = forwadRawValue >= DEAD_ZONE_VALUE ? map(forwadRawValue, 50, 1023, 1, 255) : 0;

  int backwadRawValue = analogRead(_analogMotorBackwardPin);
  _backwardReducedValue = backwadRawValue >= DEAD_ZONE_VALUE ? map(backwadRawValue, 50, 1023, 1, 255) : 0;


  /*
  // debug prints
  Serial.print("T: ");
  Serial.print(_throttleReducedValue);
  Serial.print(", F: ");
  Serial.print(_forwardReducedValue);
  Serial.print(", B: ");
  Serial.println(_backwardReducedValue);
  */

  return true;  // signal is valid
}

bool RCThrottleHandler::hasValidSignal() {
  return _hasValidSignal;
}

bool RCThrottleHandler::isMovingForward() {
  return _forwardReducedValue > 0 && _backwardReducedValue == 0;
}

bool RCThrottleHandler::isMovingBackward() {
  return _backwardReducedValue > 0 && _backwardReducedValue == 0;
}

bool RCThrottleHandler::isStationary() {
  return _throttleReducedValue == 0 && _backwardReducedValue == 0;
}

bool RCThrottleHandler::isBreaking() {
  return (_forwardReducedValue > 0 && _throttleReducedValue < BYTE_MID - THROTTLE_MIDDLE_POS_THRESHOLD)
         || (_backwardReducedValue > 0 && _throttleReducedValue > BYTE_MID + THROTTLE_MIDDLE_POS_THRESHOLD);
}

bool RCThrottleHandler::isReverse() {
  return _backwardReducedValue > 0;
}

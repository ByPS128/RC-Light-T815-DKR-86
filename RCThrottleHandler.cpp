#include <HardwareSerial.h>
#include "RCThrottleHandler.h"

RCThrottleHandler::RCThrottleHandler()
  : _hasValidSignal(false),
    _throttleChannelPwmPin(0),
    _analogMotorForwardPin(0),
    _analogMotorBackwardPin(0),
    _throttleReducedValue(0),
    _forwardSpin(0),
    _backwardSpin(false) {
  //
}

void RCThrottleHandler::init(byte throttleChannelPwmPin, byte analogMotorForwardPin, byte analogMotorBackwardPin) {
  _throttleChannelPwmPin = throttleChannelPwmPin;
  _analogMotorForwardPin = analogMotorForwardPin;
  _analogMotorBackwardPin = analogMotorBackwardPin;

  // Nastavení vlastností filtru
  noiseFilter.setProperties(6, 1000, 3); // Buffer velikost 6, práh času 1000 ms, početní práh 3
}

// Returns booleas in meaning of hasValidSignal?
bool RCThrottleHandler::update() {
  unsigned long pwmRawValue = pulseIn(_throttleChannelPwmPin, HIGH);
  _hasValidSignal = pwmRawValue >= SIGNAL_VALID_LOW_VALUE_THRESHOLD && pwmRawValue <= SIGNAL_VALID_HIGH_VALUE_THRESHOLD;
  if (!_hasValidSignal) {
    return false;  // signal is invalid
  }

  // Range bounds 1000 and 2000 will be result of callibration, in future.
  // mid = 1470
  int scaledDownPwmValue = map(pwmRawValue, 970, 1970, BYTE_MIN, BYTE_MAX);
  _throttleReducedValue = constrain(scaledDownPwmValue, BYTE_MIN, BYTE_MAX);

  _forwardSpin = digitalRead(_analogMotorForwardPin) == 0;

  int backwardRawSpinValue = analogRead(_analogMotorBackwardPin);
  bool backwardSpin = backwardRawSpinValue < 830;

  unsigned long currentMillis = millis();
  noiseFilter.addValue(currentMillis, backwardSpin);
  _backwardSpin = noiseFilter.getFilteredValue();


  // // debug prints
  // Serial.print("T: (");
  // Serial.print(pwmRawValue);
  // Serial.print(")");
  // Serial.print(_throttleReducedValue);
  // Serial.print(", F: ");
  // Serial.print(_forwardSpin);
   Serial.print(", B: ");
   Serial.println(backwardValue);

  return true;  // signal is valid
}

bool RCThrottleHandler::hasValidSignal() {
  return _hasValidSignal;
}

bool RCThrottleHandler::isMovingForward() {
  return _forwardSpin || _throttleReducedValue > 130;
}

bool RCThrottleHandler::isMovingBackward() {
  return _backwardSpin || (!_backwardSpin || _throttleReducedValue < 123);
}

bool RCThrottleHandler::isStationary() {
  return !_forwardSpin && !_backwardSpin;
}

bool RCThrottleHandler::isBreaking() {
  return !_backwardSpin && ((_forwardSpin && _throttleReducedValue < BYTE_MID - THROTTLE_MIDDLE_POS_THRESHOLD)
         || (!_forwardSpin && _throttleReducedValue < BYTE_MID - THROTTLE_MIDDLE_POS_THRESHOLD));
}

bool RCThrottleHandler::isReverse() {
  return _backwardSpin ;//|| (!_backwardSpin || _throttleReducedValue < 123);
}

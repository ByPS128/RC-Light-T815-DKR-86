#include <HardwareSerial.h>
#include "RCThrottleHandler.h"

RCThrottleHandler::RCThrottleHandler()
  : _analogMotorBackwardPin(0),
    _backwardSpin(false),
    _isBreaking(false),
    _isReverse(false)
{
}

void RCThrottleHandler::init(RCChannel* rcChannel, byte analogMotorBackwardPin) {
  _rcChannel = rcChannel;
  _analogMotorBackwardPin = analogMotorBackwardPin;
}

void RCThrottleHandler::update() {
  if (!_rcChannel->isSignalPresent()) {
    _isReverse = false;
    _isBreaking = false;

    return;
  }

  int backwardRawSpinValue = analogRead(_analogMotorBackwardPin);
  bool backwardSpin = backwardRawSpinValue < 50;

  _backwardSpin = backwardSpin;

  int pwmRawValue = _rcChannel->getValue();
  int midValue = _rcChannel->getNeutral();

  _isReverse = _backwardSpin && pwmRawValue < midValue;
  _isBreaking = !_isReverse && pwmRawValue < midValue - 100;

  // // debug prints
  // Serial.print("T: ");
  // Serial.print(pwmRawValue);
  // Serial.print(", M: ");
  // Serial.print(midValue);
  // Serial.print(", B: ");
  // Serial.println(backwardRawSpinValue);
}

bool RCThrottleHandler::isBreaking() {
  return _isBreaking;
}

bool RCThrottleHandler::isReverse() {
  return _isReverse;
}

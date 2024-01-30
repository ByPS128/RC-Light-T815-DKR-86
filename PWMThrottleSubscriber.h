#pragma once

#include <Arduino.h>
#include "AppConstants.h"

class PWMThrottleSubscriber {
public:
  PWMThrottleSubscriber();

  void init(byte throttleChannelPwmPin, byte analogMotorForwardPin, byte analogMotorBackwardPin);
  bool update();
  bool hasValidSignal();
  bool isMovingForward();
  bool isMovingBackward();
  bool isStationary();
  bool isBreaking();
  bool isReverse();

private:
    static const byte STATIONARY_MARGIN = 7; // margin pro stacionární stav
    static const byte FORWARD_THRESHOLD = BYTE_MID + STATIONARY_MARGIN;
    static const byte BACKWARD_THRESHOLD = BYTE_MID - 1 - STATIONARY_MARGIN;

private:
  bool _hasValidSignal;
  byte _throttleChannelPwmPin;
  byte _analogMotorForwardPin;
  byte _analogMotorBackwardPin;

  byte _throttleReducedValue;
  byte _forwardReducedValue;
  byte _backwardReducedValue;
};

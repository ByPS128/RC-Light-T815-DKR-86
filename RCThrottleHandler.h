#pragma once

#include <Arduino.h>
#include <math.h>
#include "AppConstants.h"
#include "RCChannel.h"

// The PWM Throttle Subscriber class reads the signal from the channel I set via init() method used as throttle/brake and evaluates its states in real time.
// The evaluated information is provided by reading methods such as isMoving.., isBreaking etc.
class RCThrottleHandler {
public:
  RCThrottleHandler();

  void init(RCChannel* rcChannel, byte analogMotorBackwardPin);
  void update();
  bool isBreaking();
  bool isReverse();

private:
  RCChannel* _rcChannel;
  byte _analogMotorBackwardPin;
  bool _backwardSpin;
  bool _isBreaking;
  bool _isReverse;
};

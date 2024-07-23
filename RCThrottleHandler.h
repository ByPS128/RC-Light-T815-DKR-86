#pragma once

#include <Arduino.h>
#include <math.h>
#include "AppConstants.h"
#include "NoiseFilter.h"

// The PWM Throttle Subscriber class reads the signal from the channel I set via init() method used as throttle/brake and evaluates its states in real time.
// The evaluated information is provided by reading methods such as isMoving.., isBreaking etc.
class RCThrottleHandler {
public:
  RCThrottleHandler();

  void init(byte throttleChannelPwmPin, byte analogMotorBackwardPin);
  bool update();
  bool hasValidSignal();
  bool isBreaking();
  bool isReverse();

private:
    static const byte DEAD_ZONE_VALUE = 12;
    static const byte DEAD_ZONE_THRESHOLD = ceil((255 / 1024) * DEAD_ZONE_VALUE);
    static const byte THROTTLE_MIDDLE_POS_THRESHOLD = ceil((255 / 1000) * 4);

private:
  bool _hasValidSignal;
  byte _throttleChannelPwmPin;
  byte _analogMotorBackwardPin;

  byte _throttleReducedValue;
  bool _backwardSpin;
  bool _isBreaking;
  bool _isReverse;

  NoiseFilter noiseFilter; // Buffer velikost 6
};

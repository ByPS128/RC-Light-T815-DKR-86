#pragma once
#include "RCChannel.h"
#include "AppConstants.h"

class SignalValidator {
public:
  SignalValidator(RCChannel* channels[AppConstants::CHANNEL_COUNT]);
  bool isSignalValid();

private:
  RCChannel* _channels[AppConstants::CHANNEL_COUNT];

  bool checkSignalPresence();
  bool checkPulseWidth();
};
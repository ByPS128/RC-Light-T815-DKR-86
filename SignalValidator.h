#pragma once
#include "RCChannel.h"
#include "constants.h"

class SignalValidator {
public:
    SignalValidator(RCChannel* channels[Constants::CHANNEL_COUNT]);
    bool isSignalValid();

private:
    RCChannel* _channels[Constants::CHANNEL_COUNT];
    unsigned long _lastPulseTime[Constants::CHANNEL_COUNT];
    bool _isPulseValid[Constants::CHANNEL_COUNT];

    bool checkPulseLoss();
    bool checkPulseWidth();
    bool checkFailsafe();
};
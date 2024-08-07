#pragma once
#include "RCChannel.h"
#include "constants.h"

class SignalValidator {
public:
    SignalValidator(RCChannel* channels[Constants::CHANNEL_COUNT]);
    bool isSignalValid();

private:
    RCChannel* _channels[Constants::CHANNEL_COUNT];

    bool checkSignalPresence();
    bool checkPulseWidth();
};
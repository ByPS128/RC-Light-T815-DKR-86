#include "SignalValidator.h"

SignalValidator::SignalValidator(RCChannel* channels[Constants::CHANNEL_COUNT]) {
    for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
        _channels[i] = channels[i];
        _lastPulseTime[i] = 0;
        _isPulseValid[i] = false;
    }
}

bool SignalValidator::isSignalValid() {
    return checkPulseLoss() && checkPulseWidth() && checkFailsafe();
}

bool SignalValidator::checkPulseLoss() {
    unsigned long currentTime = micros();
    bool isValid = true;

    for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
        if (currentTime - _lastPulseTime[i] > Constants::MAX_PULSE_GAP_US) {
            isValid = false;
            _isPulseValid[i] = false;
        } else if (_channels[i]->hasNewPulse()) {
            _lastPulseTime[i] = currentTime;
            _isPulseValid[i] = true;
        }
    }

    return isValid;
}

bool SignalValidator::checkPulseWidth() {
    for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
        int pulseWidth = _channels[i]->getValue();
        if (pulseWidth < Constants::MIN_PULSE_WIDTH_US || pulseWidth > Constants::MAX_PULSE_WIDTH_US) {
            return false;
        }
    }
    return true;
}

bool SignalValidator::checkFailsafe() {
    int failsafeCount = 0;
    for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
        if (_channels[i]->getValue() == Constants::FAILSAFE_PULSE_WIDTH_US) {
            failsafeCount++;
        }
    }
    return failsafeCount < Constants::CHANNEL_COUNT;
}
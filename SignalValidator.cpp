#include "SignalValidator.h"

SignalValidator::SignalValidator(RCChannel* channels[AppConstants::CHANNEL_COUNT]) {
  for (uint8_t i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    _channels[i] = channels[i];
  }
}

bool SignalValidator::isSignalValid() {
  return checkSignalPresence() && checkPulseWidth();
}

bool SignalValidator::checkSignalPresence() {
  for (uint8_t i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    if (!_channels[i]->isSignalPresent()) {
      return false;
    }
  }
  return true;
}

bool SignalValidator::checkPulseWidth() {
  for (uint8_t i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    uint16_t pulseWidth = _channels[i]->getValue();
    if (pulseWidth < AppConstants::MIN_PULSE_WIDTH_US || pulseWidth > AppConstants::MAX_PULSE_WIDTH_US) {
      return false;
    }
  }
  return true;
}
#include "RCChannel.h"
#include "constants.h"

RCChannel::RCChannel(int pin) : _pin(pin), _isCalibrated(false) {
    pinMode(_pin, INPUT);
}

void RCChannel::update() {
    int newValue = pulseIn(_pin, HIGH);
    if (newValue != _value) {
        _value = newValue;
        _newPulse = true;
    }
}

int RCChannel::getValue() const {
    return _value;
}

void RCChannel::calibrate(int min, int max, int neutral) {
    _min = min;
    _max = max;
    _neutral = neutral;
    _isCalibrated = true;
}

bool RCChannel::isCalibrated() const {
    return _isCalibrated;
}

int RCChannel::getMin() const {
    return _min;
}

int RCChannel::getMax() const {
    return _max;
}

int RCChannel::getNeutral() const {
    return _neutral;
}

bool RCChannel::hasNewPulse() {
    if (_newPulse) {
        _newPulse = false;
        return true;
    }
    return false;
}

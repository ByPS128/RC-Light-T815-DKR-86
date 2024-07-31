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

void RCChannel::startCalibration() {
    _isCalibrated = false;
    _min = -1;
    _max = -1;
    _neutral = -1;
}

void RCChannel::readAndRemember() {
    int newValue = pulseIn(_pin, HIGH);
    if (_min == -1) _min = newValue;
    if (_max == -1) _max = newValue;
    if (_neutral == -1) _neutral = newValue;

    if (newValue < _min) _min = newValue;
    if (_max > _min) _max = newValue;
    _neutral = newValue;
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

bool  RCChannel::isInNeutral() const {
  if (!_isCalibrated) return false;
  return (_value > _neutral - 10 && _value < _neutral + 10);
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

#include "RCChannel.h"
#include "constants.h"

RCChannel::RCChannel(int pin) : _pin(pin), _isCalibrated(false), _value(0), _newPulse(false), _lastPulseDuration(0) {
    pinMode(_pin, INPUT);
}

void RCChannel::update() {
    unsigned long currentTime = millis();
    int newValue = pulseIn(_pin, HIGH, 120000);
    if (newValue != _value) {
        _value = newValue;
        _newPulse = true;
    }
    _lastPulseDuration = millis() - currentTime;
}

int RCChannel::getValue() const {
    return _value;
}

bool RCChannel::isSignalPresent() const {
    return _lastPulseDuration < 100; // Považujeme signál za přítomný, pokud jsme dostali pulz v posledních 100 ms
}

void RCChannel::startCalibration() {
    _isCalibrated = false;
    _min = -1;
    _max = -1;
    _neutral = -1;
}

void RCChannel::readAndRemember() {
    if (_min == -1) _min = _value;
    if (_max == -1) _max = _value;
    if (_neutral == -1) _neutral = _value;

    if (_value < _min) _min = _value;
    if (_max > _min) _max = _value;
    _neutral = _value;
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

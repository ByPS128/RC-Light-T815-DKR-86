#include "RCChannel.h"
#include "AppConstants.h"

RCChannel::RCChannel(int pin)
  : _pin(pin), _min(0), _max(0), _neutral(0), _value(0), _isCalibrated(false), 
    _newPulse(false), _lastPulseDuration(0), _tolerance(POSITION_TOLERANCE),
    _orgMin(0), _orgMax(0), _orgNeutral(0), _orgIsCalibrated(false) {
  pinMode(_pin, INPUT);
}

void RCChannel::update() {
  // Měřím šířku pulzu a aktualizuji hodnotu kanálu
  unsigned long currentTime = millis();
  int newValue = pulseIn(_pin, HIGH, 120000);
  _lastPulseDuration = millis() - currentTime;

  if (newValue != _value) {
    _value = newValue;
    _newPulse = true;
  }
}

void RCChannel::setTolerance(int tolerance) {
  _tolerance = tolerance;
}

int RCChannel::getValue() const {
  return _value;
}

bool RCChannel::isSignalPresent() const {
  // Kontroluji, zda byl signál přítomen v posledních 100 ms
  return _lastPulseDuration < 100;
}

void RCChannel::startCalibration() {
  // Zahajuji kalibraci resetováním hodnot
  _isCalibrated = false;

  _orgMin = _min;
  _orgMax = _max;
  _orgNeutral = _neutral;
  _orgIsCalibrated = _isCalibrated;

  _min = -1;
  _max = -1;
  _neutral = -1;
}

void RCChannel::restoreCalibration() {
  _min = _orgMin;
  _max = _orgMax;
  _neutral = _orgNeutral;
  _isCalibrated = _orgIsCalibrated;
}

void RCChannel::readAndRemember() {
  // Čtu a ukládám kalibrační hodnoty
  if (_min == -1) _min = _value;
  if (_max == -1) _max = _value;
  if (_neutral == -1) _neutral = _value;

  if (_value < _min) _min = _value;
  if (_value > _max) _max = _value;
  _neutral = _value;
}

void RCChannel::calibrate(int min, int max, int neutral) {
  // Nastavuji kalibrační hodnoty a označuji kanál jako kalibrovaný
  _min = min;
  _max = max;
  _neutral = neutral;
  _isCalibrated = true;
}

bool RCChannel::isCalibrated() const {
  return _isCalibrated;
}

bool RCChannel::isInNeutral() const {
  // Kontroluji, zda je hodnota v neutrální pozici s danou tolerancí
  if (!_isCalibrated) return false;
  return (_value > _neutral - _tolerance && _value < _neutral + _tolerance);
}

bool RCChannel::isInMin() const {
  // Kontroluji, zda je hodnota v minimální pozici s danou tolerancí
  if (!_isCalibrated) return false;
  return (_value > _min - _tolerance && _value < _min + _tolerance);
}

bool RCChannel::isInMax() const {
  // Kontroluji, zda je hodnota v maximální pozici s danou tolerancí
  if (!_isCalibrated) return false;
  return (_value > _max - _tolerance && _value < _max + _tolerance);
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
  // Kontroluji a resetuji příznak nového pulzu
  if (_newPulse) {
    _newPulse = false;
    return true;
  }
  return false;
}

String RCChannel::getNamedPosition() {
  // Určuji pojmenovanou pozici kanálu
  if (!_isCalibrated) return "---";
  if (isInMin()) return "MIN";
  if (isInMax()) return "MAX";
  if (isInNeutral()) return "MID";
  return " . ";  // Pokud není v žádné z definovaných pozic
}
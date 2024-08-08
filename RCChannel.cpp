#include "RCChannel.h"
#include "AppConstants.h"

RCChannel::RCChannel(uint8_t pin)
  : _pin(pin), _min(0), _max(0), _neutral(0), _value(0), _isCalibrated(false),
    _newPulse(false), _lastPulseDuration(0), _tolerance(POSITION_TOLERANCE),
    _orgMin(0), _orgMax(0), _orgNeutral(0), _orgIsCalibrated(false) {
  pinMode(_pin, INPUT);
}

void RCChannel::update() {
  // Měřím šířku pulzu a aktualizuji hodnotu kanálu
  unsigned long currentTime = millis();
  uint16_t newValue = pulseIn(_pin, HIGH, 120000);
  _lastPulseDuration = millis() - currentTime;

  if (newValue != _value) {
    _value = newValue;
    _newPulse = true;
  }
}

void RCChannel::startCalibration() {
  // Zahajuji kalibraci resetováním hodnot
  _isCalibrated = false;

  _orgMin = _min;
  _orgMax = _max;
  _orgNeutral = _neutral;
  _orgIsCalibrated = _isCalibrated;

  _min = 0;
  _max = 0;
  _neutral = 0;
}

void RCChannel::restoreCalibration() {
  _min = _orgMin;
  _max = _orgMax;
  _neutral = _orgNeutral;
  _isCalibrated = _orgIsCalibrated;
}

void RCChannel::readAndRemember() {
  // Čtu a ukládám kalibrační hodnoty
  if (_min == 0) _min = _value;
  if (_max == 0) _max = _value;
  if (_neutral == 0) _neutral = _value;

  if (_value < _min) _min = _value;
  if (_value > _max) _max = _value;
  _neutral = _value;
}

void RCChannel::calibrate(uint16_t min, uint16_t max, uint16_t neutral) {
  // Nastavuji kalibrační hodnoty a označuji kanál jako kalibrovaný
  _min = min;
  _max = max;
  _neutral = neutral;
  _isCalibrated = true;
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
  static const char unknown[] PROGMEM = "---";
  static const char min[] PROGMEM = "MIN";
  static const char max[] PROGMEM = "MAX";
  static const char middle[] PROGMEM = "MID";
  static const char between[] PROGMEM = " - ";

  // Určuji pojmenovanou pozici kanálu
  if (!_isCalibrated) return reinterpret_cast<const __FlashStringHelper*>(unknown);
  if (isInMin()) return reinterpret_cast<const __FlashStringHelper*>(min);
  if (isInMax()) return reinterpret_cast<const __FlashStringHelper*>(max);
  if (isInNeutral()) return reinterpret_cast<const __FlashStringHelper*>(middle);
  return reinterpret_cast<const __FlashStringHelper*>(between);  // Pokud není v žádné z definovaných pozic
}
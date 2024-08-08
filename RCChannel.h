#pragma once
#include <Arduino.h>

class RCChannel {
public:
  static const uint8_t POSITION_TOLERANCE = 10;  // Tolerance pro určení pozice v mikrosekundách

  RCChannel(uint8_t pin);
  void update();
  inline void setTolerance(uint16_t tolerance) { _tolerance = tolerance; }
  inline uint16_t getValue() const { return _value; }
  void startCalibration();
  void restoreCalibration();
  void readAndRemember();
  void calibrate(uint16_t min, uint16_t max, uint16_t neutral);
  inline bool isCalibrated() const { return _isCalibrated; }
  inline bool isInNeutral() const { return _isCalibrated && (_value > _neutral - _tolerance && _value < _neutral + _tolerance); }
  inline bool isInMin() const { return _isCalibrated && (_value > _min - _tolerance && _value < _min + _tolerance); }
  inline bool isInMax() const { return _isCalibrated && (_value > _max - _tolerance && _value < _max + _tolerance); }
  inline uint16_t getMin() const { return _min; }
  inline uint16_t getMax() const { return _max; }
  inline uint16_t getNeutral() const { return _neutral; }
  bool hasNewPulse();
  inline bool isSignalPresent() const { return _lastPulseDuration < 100; }
  String getNamedPosition();

private:
  uint8_t _pin;
  uint16_t _min;
  uint16_t _max;
  uint16_t _neutral;
  uint16_t _value;
  bool _isCalibrated;
  bool _newPulse;
  uint16_t _lastPulseDuration;
  uint16_t _tolerance;
  uint16_t _orgMin;
  uint16_t _orgMax;
  uint16_t _orgNeutral;
  bool _orgIsCalibrated;
};
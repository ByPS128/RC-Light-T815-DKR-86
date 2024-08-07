#pragma once
#include <Arduino.h>

class RCChannel {
public:
  static const int POSITION_TOLERANCE = 10;  // Tolerance pro určení pozice v mikrosekundách

  RCChannel(int pin);
  void update();
  int getValue() const;
  void startCalibration();
  void readAndRemember();
  void calibrate(int min, int max, int neutral);
  bool isCalibrated() const;
  bool isInNeutral() const;
  bool isInMin() const;
  bool isInMax() const;
  int getMin() const;
  int getMax() const;
  int getNeutral() const;
  bool hasNewPulse();
  bool isSignalPresent() const;
  String getNamedPosition();

private:
  int _pin;
  int _min;
  int _max;
  int _neutral;
  int _value;
  bool _isCalibrated;
  bool _newPulse;
  int _lastPulseDuration;
};
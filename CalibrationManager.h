#pragma once
#include <Arduino.h>
#include "RCChannel.h"
#include "constants.h"

class CalibrationManager {
public:
  CalibrationManager(RCChannel* channels[Constants::CHANNEL_COUNT]);
  void begin();
  void update();
  bool isCalibrated() const;
  void startCalibration();
  void saveCalibration();
  void loadCalibration();
  void clearEEPROM();
	void turnCalibrationMode();

private:
  RCChannel* _channels[Constants::CHANNEL_COUNT];
  bool _isCalibrating;
  unsigned long _calibrationStartTime;
  int _calibrationStep;

  void updateCalibration();
  void finishCalibration();
};
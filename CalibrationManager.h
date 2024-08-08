#pragma once
#include <Arduino.h>
#include "RCChannel.h"
#include "AppConstants.h"

class CalibrationManager {
public:
  CalibrationManager(RCChannel* channels[AppConstants::CHANNEL_COUNT]);
  void init();
  void update();
  bool isCalibrated() const;
  void startCalibration();
  void saveCalibration();
  void loadCalibration();
  void clearEEPROM();
  void turnCalibrationMode();
  bool isInCalibrationMode();
  void terminateCalibrationMode();

private:
  RCChannel* _channels[AppConstants::CHANNEL_COUNT];
  bool _isCalibrating;
  unsigned long _calibrationStartTime;
  uint8_t _calibrationStep;

  uint16_t EEPROMReadInt(uint16_t address);
  void EEPROMWriteInt(uint16_t address, uint16_t value);
  void updateCalibration();
  void finishCalibration();
};
#pragma once
#include <Arduino.h>
#include "RCChannel.h"
#include "constants.h"

class CalibrationManager {
public:
    CalibrationManager(RCChannel* channels[Constants::CHANNEL_COUNT], int calibrationPin, int ledPin);
    void begin();
    void update();
    bool isCalibrated() const;
    void startCalibration();
    void saveCalibration();
    void loadCalibration();

private:
    RCChannel* _channels[Constants::CHANNEL_COUNT];
    int _calibrationPin;
    int _ledPin;
    bool _isCalibrating;
    unsigned long _calibrationStartTime;
    int _calibrationStep;

    void updateCalibration();
    void finishCalibration();
    void signalUncalibratedState();
};
#pragma once
#include <Arduino.h>
#include "RCChannel.h"
#include "DigitalPullUpButton.h"
#include "constants.h"

class CalibrationManager : public IButtonEventSubscriber {
public:
    CalibrationManager(RCChannel* channels[Constants::CHANNEL_COUNT], int calibrationPin, int ledPin);
    void begin();
    void update();
    bool isCalibrated() const;
    void startCalibration();
    void saveCalibration();
    void loadCalibration();
    void onButtonClick(int buttonId, ButtonClickType clickKind) override;

private:
    RCChannel* _channels[Constants::CHANNEL_COUNT];
    DigitalPullUpButton calibrationButton;
    int _calibrationPin;
    int _ledPin;
    bool _isCalibrating;
    unsigned long _calibrationStartTime;
    int _calibrationStep;

    void updateCalibration();
    void finishCalibration();
    void signalUncalibratedState();
	
	void onCalibrationButtonClick(ButtonClickType clickKind);
};
#pragma once
#include <Arduino.h>

class RCChannel {
public:
    RCChannel(int pin);
    void update();
    int getValue() const;
    void calibrate(int min, int max, int neutral);
    bool isCalibrated() const;
    int getMin() const;
    int getMax() const;
    int getNeutral() const;
	bool hasNewPulse();

private:
    int _pin;
    int _value;
    int _min;
    int _max;
    int _neutral;
    bool _isCalibrated;
	bool _newPulse;
};
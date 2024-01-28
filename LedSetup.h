#pragma once

#include <Arduino.h>
#include "LedControlBlinker.h"

class LedSetup {
private:
  byte pwmInSteeringPin;
  byte ledPin;
  byte ledBrightness;
  long pwmSteeringValue;
  long pwmSteeringValueMin = NO_VALUE;
  long pwmSteeringValueMax = NO_VALUE;

public:
  LedSetup();
  void init(byte pwmInSteeringPin, byte ledPin);
  void updateCalibration();
  void updateBrightnessAdjustment();
  void setRangeLimits(int pwmRangeLow, int pwmRangeHigh);
  void resetRangeLimits();
  void setLedBrightness(byte ledBrightness);
  int getLowRangeLimit();
  int getHighRangeLimit();
  byte getLedBrightness();

private:
  const int NO_VALUE = -99999;

private:
  void readChanelsValues();
  void processButton();
};

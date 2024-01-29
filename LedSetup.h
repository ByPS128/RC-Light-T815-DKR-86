#pragma once

#include <Arduino.h>
#include "LedControlBlinker.h"

class LedSetup {
private:
  const long NO_VALUE = -99999;

private:
  byte pwmInSteeringPin;
  byte pwmLightPin;
  byte ledPin;
  byte ledBrightness;
  long pwmSteeringValue;
  long pwmSteeringValueMin = NO_VALUE;
  long pwmSteeringValueMax = NO_VALUE;

public:
  LedSetup();
  void init(byte pwmInSteeringPin, byte pwmLightPin, byte ledPin);
  void updateCalibration();
  void updateBrightnessAdjustment();
  void setRangeLimits(int pwmRangeLow, int pwmRangeHigh);
  void resetRangeLimits();
  void setLedBrightness(byte ledBrightness);
  int getLowRangeLimit();
  int getHighRangeLimit();
  byte getLedBrightness();

private:
  void readChanelsValues();
  void processButton();
};

#pragma once

#include <Arduino.h>
#include "RCChannel.h"

class RCSteeringHandler {
private:
  RCChannel* _rcChannel;
  byte pwmLightPin;
  byte ledPin;
  byte ledBrightness;

public:
  RCSteeringHandler();
  void init(RCChannel* rcChannel, byte pwmLightPin, byte ledPin);
  void updateBrightnessAdjustment();
  byte getLedBrightness();
  void setLedBrightness(byte ledBrightness);

private:
  void readChanelsValues();
  void processButton();
  void describe();
};

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
  inline uint8_t getLedBrightness() { return ledBrightness; }
  inline void setLedBrightness(uint8_t ledBrightness) { this->ledBrightness = ledBrightness; }

private:
  void readChanelsValues();
  void describe();
};

#pragma once

#include <Arduino.h>
#include "AppConstants.h"

class LightsController {
public:
  static constexpr byte MODE_NONE = 0;
  static constexpr byte MODE_LIGHT_1 = 1;
  static constexpr byte MODE_LIGHT_2 = 2;
  static constexpr byte MODE_LIGHT_3 = 3;

  static constexpr byte MODE_MIN = MODE_NONE;
  static constexpr byte MODE_MAX = MODE_LIGHT_3;

public:
  LightsController();

  void init(byte lightMode, byte ledBrightness, byte pwmLight1Pin, byte pwmLight2Pin, byte pwmLight3Pin, byte pwmLightBrakePin, byte digitalLightBrakePin, byte digitalReversePin);
  void turnToNextLightMode();
  byte getCurrentLightMode();
  void setBreaking(bool state);
  void setReverse(bool state);
  void turnMaximumLights();
  void setLightsPinsByCurrentMode();
  void setLedBirigthness(byte newLedBrightness);

private:
  const byte LIGHT_MATRIX[4][3] = {
    { 0, 0, 0 },  // Mode No Lights
    { 1, 0, 0 },  // Mode Outer Day Light Only
    { 1, 1, 0 },  // Mode Outer Day Light Plus Long Light
    { 1, 1, 1 },  // Mode All Lights
  };

private:
  byte pwmLight1Pin;
  byte pwmLight2Pin;
  byte pwmLight3Pin;
  byte pwmLightBrakePin;
  byte digitalLightBrakePin;
  byte digitalReversePin;
  byte currentLightMode;
  bool isBreaking;
  bool isReverse;
  bool isFullLightMode;
  byte ledBrightness;
};

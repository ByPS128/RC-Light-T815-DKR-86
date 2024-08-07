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

  void init(byte lightMode, byte ledBrightness, byte pwmFrontLightsPin,
            byte digitalLight1Pin, byte digitalLight2Pin, byte digitalLight3Pin,
            byte digitalOuterBrakePin, byte digitalOuterBrakeModePin,
            byte digitalInnerBrakePin, byte digitalReversePin);
  void turnToNextLightMode();
  byte getCurrentLightMode();
  bool setBreaking(bool state);
  bool setReverse(bool state);
  void turnMaximumLights();
  void setLightsPinsByCurrentMode();
  void setLedBirigthness(byte newLedBrightness);
  void setLightsMode(byte mode);

private:
  const byte LIGHT_MATRIX[4][3] = {
    { 0, 0, 0 },  // Mode No Lights
    { 1, 0, 0 },  // Mode Outer Day Light Only
    { 1, 1, 0 },  // Mode Outer Day Light Plus Long Light
    { 1, 1, 1 },  // Mode All Lights
  };

private:
  byte pwmFrontLightsPin;
  byte digitalLight1Pin;
  byte digitalLight2Pin;
  byte digitalLight3Pin;
  byte digitalOuterBrakePin;
  byte digitalOuterBrakeModePin;
  byte digitalInnerBrakePin;
  byte digitalReversePin;
  byte currentLightMode;
  bool isBreaking;
  bool isReverse;
  bool isFullLightMode;
  byte ledBrightness;
};

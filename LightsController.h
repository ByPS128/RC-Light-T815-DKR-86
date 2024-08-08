#include <stdint.h>
#include <avr/pgmspace.h>
#pragma once

#include <Arduino.h>
#include "AppConstants.h"

class LightsController {
public:
  static constexpr uint8_t MODE_NONE = 0;
  static constexpr uint8_t MODE_LIGHT_1 = 1;
  static constexpr uint8_t MODE_LIGHT_2 = 2;
  static constexpr uint8_t MODE_LIGHT_3 = 3;

  static constexpr uint8_t MODE_MIN = MODE_NONE;
  static constexpr uint8_t MODE_MAX = MODE_LIGHT_3;

public:
  LightsController();

  void init(uint8_t lightMode, uint8_t ledBrightness, uint8_t pwmFrontLightsPin,
            uint8_t digitalLight1Pin, uint8_t digitalLight2Pin, uint8_t digitalLight3Pin,
            uint8_t digitalOuterBrakePin, uint8_t digitalOuterBrakeModePin,
            uint8_t digitalInnerBrakePin, uint8_t digitalReversePin);
  void turnToNextLightMode();
  uint8_t getCurrentLightMode();
  bool setBreaking(bool state);
  bool setReverse(bool state);
  void turnMaximumLights();
  void setLightsPinsByCurrentMode();
  void setLedBirigthness(uint8_t newLedBrightness);
  void setLightsMode(uint8_t mode);

private:
  static const uint8_t LIGHT_MATRIX[4][3];

private:
  uint8_t pwmFrontLightsPin;
  uint8_t digitalLight1Pin;
  uint8_t digitalLight2Pin;
  uint8_t digitalLight3Pin;
  uint8_t digitalOuterBrakePin;
  uint8_t digitalOuterBrakeModePin;
  uint8_t digitalInnerBrakePin;
  uint8_t digitalReversePin;
  uint8_t currentLightMode;
  bool isBreaking;
  bool isReverse;
  bool isFullLightMode;
  uint8_t ledBrightness;
};

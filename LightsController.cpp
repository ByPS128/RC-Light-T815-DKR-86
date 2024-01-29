#include "LightsController.h"


LightsController::LightsController()
  : pwmFrontLightsPin(0), digitalLight1Pin(0), digitalLight2Pin(0), digitalLight3Pin(0), pwmLightBrakePin(0), digitalLightBrakePin(0),
    digitalReversePin(0), currentLightMode(MODE_NONE), isBreaking(false), isReverse(false),
    isFullLightMode(false), ledBrightness(0) {
}

void LightsController::init(byte lightMode, byte ledBrightness, byte pwmFrontLightsPin, byte digitalLight1Pin, byte digitalLight2Pin, byte digitalLight3Pin, byte pwmLightBrakePin, byte digitalLightBrakePin, byte digitalReversePin) {
  currentLightMode = lightMode;
  this->ledBrightness = ledBrightness;
  this->pwmFrontLightsPin = pwmFrontLightsPin;
  this->digitalLight1Pin = digitalLight1Pin;
  this->digitalLight2Pin = digitalLight2Pin;
  this->digitalLight3Pin = digitalLight3Pin;
  this->pwmLightBrakePin = pwmLightBrakePin;
  this->digitalLightBrakePin = digitalLightBrakePin;
  this->digitalReversePin = digitalReversePin;
  isBreaking = false;
  isReverse = false;
  isFullLightMode = false;
  setLightsPinsByCurrentMode();
}

void LightsController::turnToNextLightMode() {
  currentLightMode = (currentLightMode + 1) % (MODE_MAX + 1);
  setLightsPinsByCurrentMode();
}

byte LightsController::getCurrentLightMode() {
  return currentLightMode;
}

void LightsController::setBreaking(bool state) {
  bool oldIsBreaking = isBreaking;
  isBreaking = state;
  if (isBreaking != oldIsBreaking) {
    setLightsPinsByCurrentMode();
  }
}

void LightsController::setReverse(bool state) {
  bool oldIsReverse = isReverse;
  isReverse = state;
  if (isReverse != oldIsReverse) {
    setLightsPinsByCurrentMode();
  }
}

void LightsController::turnMaximumLights() {
  isFullLightMode = !isFullLightMode;
  setLightsPinsByCurrentMode();
}

void LightsController::setLightsPinsByCurrentMode() {
  analogWrite(pwmLightBrakePin, isBreaking ? BYTE_MAX : LIGHT_MATRIX[currentLightMode][0] == 1 ? BYTE_MID
                                                                                               : BYTE_MIN);
  digitalWrite(digitalLightBrakePin, isBreaking ? BYTE_MAX : BYTE_MIN);
  digitalWrite(digitalReversePin, isReverse ? BYTE_MAX : BYTE_MIN);

  if (isFullLightMode) {
    analogWrite(pwmFrontLightsPin, BYTE_MAX);
    digitalWrite(digitalLight1Pin, BYTE_MAX);
    digitalWrite(digitalLight2Pin, BYTE_MAX);
    digitalWrite(digitalLight3Pin, BYTE_MAX);
    return;
  }

  //analogWrite(pwmFrontLightsPin, LIGHT_MATRIX[currentLightMode][0] == 1 || LIGHT_MATRIX[currentLightMode][1] == 1 || LIGHT_MATRIX[currentLightMode][2] == 1 ? ledBrightness : BYTE_MIN);
  analogWrite(pwmFrontLightsPin, ledBrightness);
  digitalWrite(digitalLight1Pin, LIGHT_MATRIX[currentLightMode][0] == 1 ? BYTE_MAX : BYTE_MIN);
  digitalWrite(digitalLight2Pin, LIGHT_MATRIX[currentLightMode][1] == 1 ? BYTE_MAX : BYTE_MIN);
  digitalWrite(digitalLight3Pin, LIGHT_MATRIX[currentLightMode][2] == 1 ? BYTE_MAX : BYTE_MIN);
}

void LightsController::setLedBirigthness(byte newLedBrightness) {
  ledBrightness = newLedBrightness;
  setLightsPinsByCurrentMode();
}

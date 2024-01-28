#include "LightsController.h"


LightsController::LightsController()
  : pwmLight1Pin(0), pwmLight2Pin(0), pwmLight3Pin(0), pwmLightBrakePin(0), digitalLightBrakePin(0),
    digitalReversePin(0), currentLightMode(MODE_NONE), isBreaking(false), isReverse(false),
    isFullLightMode(false), ledBrightness(0) {
}

void LightsController::init(byte lightMode, byte ledBrightness, byte pwmLight1Pin, byte pwmLight2Pin, byte pwmLight3Pin, byte pwmLightBrakePin, byte digitalLightBrakePin, byte digitalReversePin) {
  currentLightMode = lightMode;
  this->ledBrightness = ledBrightness;
  this->pwmLight1Pin = pwmLight1Pin;
  this->pwmLight2Pin = pwmLight2Pin;
  this->pwmLight3Pin = pwmLight3Pin;
  this->pwmLightBrakePin = pwmLightBrakePin;
  this->digitalLightBrakePin = digitalLightBrakePin;
  this->digitalReversePin = digitalReversePin;
  isBreaking = false;
  isReverse = false;
  isFullLightMode = false;
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
    analogWrite(pwmLight1Pin, BYTE_MAX);
    analogWrite(pwmLight2Pin, BYTE_MAX);
    analogWrite(pwmLight3Pin, BYTE_MAX);
    Serial.print("max mode: ");
    Serial.println(ledBrightness);
    return;
  }
  analogWrite(pwmLight1Pin, LIGHT_MATRIX[currentLightMode][0] == 1 ? ledBrightness : BYTE_MIN);
  analogWrite(pwmLight2Pin, LIGHT_MATRIX[currentLightMode][1] == 1 ? ledBrightness : BYTE_MIN);
  analogWrite(pwmLight3Pin, LIGHT_MATRIX[currentLightMode][2] == 1 ? ledBrightness : BYTE_MIN);
    Serial.print("normal mode: ");
    Serial.println(ledBrightness);
}

void LightsController::setLedBirigthness(byte newLedBrightness) {
  ledBrightness = newLedBrightness;
  setLightsPinsByCurrentMode();
}

#include "LightsController.h"

LightsController::LightsController()
  : pwmFrontLightsPin(0), digitalLight1Pin(0), digitalLight2Pin(0), digitalLight3Pin(0), digitalOuterBrakePin(0), digitalOuterBrakeModePin(0), 
    digitalInnerBrakePin(0), digitalReversePin(0), currentLightMode(MODE_NONE), isBreaking(false), isReverse(false),
    isFullLightMode(false), ledBrightness(0) {
}

void LightsController::init(byte lightMode, byte ledBrightness, byte pwmFrontLightsPin, 
                            byte digitalLight1Pin, byte digitalLight2Pin, byte digitalLight3Pin, 
                            byte digitalOuterBrakePin, byte digitalOuterBrakeModePin, 
                            byte digitalInnerBrakePin, byte digitalReversePin) {
  currentLightMode = lightMode;
  this->ledBrightness = ledBrightness;
  this->pwmFrontLightsPin = pwmFrontLightsPin;
  this->digitalLight1Pin = digitalLight1Pin;
  this->digitalLight2Pin = digitalLight2Pin;
  this->digitalLight3Pin = digitalLight3Pin;
  this->digitalOuterBrakeModePin = digitalOuterBrakeModePin;
  this->digitalOuterBrakePin = digitalOuterBrakePin;
  this->digitalInnerBrakePin = digitalInnerBrakePin;
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

bool LightsController::setBreaking(bool state) {
  bool oldIsBreaking = isBreaking;
  isBreaking = state;

  return isBreaking != oldIsBreaking;
}

bool LightsController::setReverse(bool state) {
  bool oldIsReverse = isReverse;
  isReverse = state;
  return isReverse != oldIsReverse;
}

void LightsController::turnMaximumLights() {
  isFullLightMode = !isFullLightMode;
  setLightsPinsByCurrentMode();
}

void LightsController::setLightsPinsByCurrentMode() {
  bool lightsAreOn = LIGHT_MATRIX[currentLightMode][0] == 1;
  // Serial.print("is reverse: ");
  // Serial.print(isReverse);
  // Serial.print(", is breaking: ");
  // Serial.println(isBreaking);

  // Reverse white light - uses full brightness
  digitalWrite(digitalReversePin, isReverse ? BYTE_MAX : BYTE_MIN);

  // Inner only brake light
  digitalWrite(digitalInnerBrakePin, isBreaking ? BYTE_MAX : BYTE_MIN); // Turned ON when breaking only.

  // Outer brake light, it is combined brake and day lights (dimmed).
  digitalWrite(digitalOuterBrakePin, lightsAreOn || isBreaking ? BYTE_MAX : BYTE_MIN); // Tirned on when lights are on or when breaking.
  digitalWrite(digitalOuterBrakeModePin, isBreaking ? BYTE_MAX : BYTE_MIN); // If breaking, shine on max, otherwise dimmed.

  if (isFullLightMode) {
    // Set PWM to full brightness of lights circuit
    analogWrite(pwmFrontLightsPin, BYTE_MAX);
    // All front white lights set ON
    digitalWrite(digitalLight1Pin, BYTE_MAX);
    digitalWrite(digitalLight2Pin, BYTE_MAX);
    digitalWrite(digitalLight3Pin, BYTE_MAX);
    return;
  }

  // Set PWM custom brightness of lights circuit
  analogWrite(pwmFrontLightsPin, ledBrightness);
  // Front white lights set ON by mode
  digitalWrite(digitalLight1Pin, LIGHT_MATRIX[currentLightMode][0] == 1 ? BYTE_MAX : BYTE_MIN);
  digitalWrite(digitalLight2Pin, LIGHT_MATRIX[currentLightMode][1] == 1 ? BYTE_MAX : BYTE_MIN);
  digitalWrite(digitalLight3Pin, LIGHT_MATRIX[currentLightMode][2] == 1 ? BYTE_MAX : BYTE_MIN);
}

void LightsController::setLedBirigthness(byte newLedBrightness) {
  ledBrightness = newLedBrightness;
  setLightsPinsByCurrentMode();
}

void LightsController::setLightsMode(byte mode) {
  currentLightMode = mode > MODE_MAX ? MODE_MAX : mode;
  setLightsPinsByCurrentMode();
}

#include "LightsController.h"

// Definice statického člena třídy
const uint8_t PROGMEM LightsController::LIGHT_MATRIX[4][3] = {
    { 0, 0, 0 },  // Mode No Lights
    { 1, 0, 0 },  // Mode Outer Day Light Only
    { 1, 1, 0 },  // Mode Outer Day Light Plus Long Light
    { 1, 1, 1 },  // Mode All Lights
};

LightsController::LightsController()
  : pwmFrontLightsPin(0), digitalLight1Pin(0), digitalLight2Pin(0), digitalLight3Pin(0), digitalOuterBrakePin(0), digitalOuterBrakeModePin(0),
    digitalInnerBrakePin(0), digitalReversePin(0), currentLightMode(MODE_NONE), isBreaking(false), isReverse(false),
    isFullLightMode(false), ledBrightness(0) {
}

void LightsController::init(uint8_t lightMode, uint8_t ledBrightness, uint8_t pwmFrontLightsPin,
                            uint8_t digitalLight1Pin, uint8_t digitalLight2Pin, uint8_t digitalLight3Pin,
                            uint8_t digitalOuterBrakePin, uint8_t digitalOuterBrakeModePin,
                            uint8_t digitalInnerBrakePin, uint8_t digitalReversePin) {
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
  bool lightsAreOn = pgm_read_byte(&LIGHT_MATRIX[currentLightMode][0]) == 1;
  // Serial.print(F("is reverse: "));
  // Serial.print(isReverse);
  // Serial.print(F(", is breaking: "));
  // Serial.println(isBreaking);

  // Reverse white light - uses full brightness
  digitalWrite(digitalReversePin, isReverse ? HIGH : LOW);

  // Inner only brake light
  digitalWrite(digitalInnerBrakePin, isBreaking ? HIGH : LOW);  // Turned ON when breaking only.

  // Outer brake light, it is combined brake and day lights (dimmed).
  digitalWrite(digitalOuterBrakePin, lightsAreOn || isBreaking ? HIGH : LOW);  // Turned on when front lights are ON or when breaking.
  digitalWrite(digitalOuterBrakeModePin, isBreaking ? HIGH : LOW);             // If breaking, shine on max, otherwise dimmed.

  if (isFullLightMode) {
    // Set PWM to full brightness of lights circuit
    analogWrite(pwmFrontLightsPin, AppConstants::BYTE_MAX);
    // All front white lights set ON
    digitalWrite(digitalLight1Pin, HIGH);
    digitalWrite(digitalLight2Pin, HIGH);
    digitalWrite(digitalLight3Pin, HIGH);
    return;
  }

  // Set PWM custom brightness of lights circuit
  analogWrite(pwmFrontLightsPin, ledBrightness);
  // Front white lights set ON by mode
  digitalWrite(digitalLight1Pin, pgm_read_byte(&LIGHT_MATRIX[currentLightMode][0]) == 1 ? HIGH : LOW);
  digitalWrite(digitalLight2Pin, pgm_read_byte(&LIGHT_MATRIX[currentLightMode][1]) == 1 ? HIGH : LOW);
  digitalWrite(digitalLight3Pin, pgm_read_byte(&LIGHT_MATRIX[currentLightMode][2]) == 1 ? HIGH : LOW);
}

void LightsController::setLedBirigthness(uint8_t newLedBrightness) {
  ledBrightness = newLedBrightness;
  setLightsPinsByCurrentMode();
}

void LightsController::setLightsMode(uint8_t mode) {
  currentLightMode = mode > MODE_MAX ? MODE_MAX : mode;
  setLightsPinsByCurrentMode();
}

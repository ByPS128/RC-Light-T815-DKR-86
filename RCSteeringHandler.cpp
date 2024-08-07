#include <Arduino.h>
#include "RCSteeringHandler.h"
#include "AppConstants.h"

RCSteeringHandler::RCSteeringHandler()
  : pwmLightPin(0), ledPin(0), ledBrightness(AppConstants::BYTE_MID) {}

void RCSteeringHandler::init(RCChannel* rcChannel, byte pwmLightPin, byte ledPin) {
  _rcChannel = rcChannel;
  this->pwmLightPin = pwmLightPin;
  this->ledPin = ledPin;
}

void RCSteeringHandler::updateBrightnessAdjustment() {
  ledBrightness = map(_rcChannel->getValue(), _rcChannel->getMin(), _rcChannel->getMax(), AppConstants::BYTE_MIN, AppConstants::BYTE_MAX);
  ledBrightness = constrain(ledBrightness, AppConstants::BYTE_MIN, AppConstants::BYTE_MAX);
  analogWrite(pwmLightPin, ledBrightness);       // Set the light intenzity
  digitalWrite(ledPin, AppConstants::BYTE_MAX);  // Turn on selected circuit
}

byte RCSteeringHandler::getLedBrightness() {
  return ledBrightness;
}

void RCSteeringHandler::setLedBrightness(byte ledBrightness) {
  this->ledBrightness = ledBrightness;
}

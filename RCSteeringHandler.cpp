#include <Arduino.h>
#include "RCSteeringHandler.h"
#include "AppConstants.h"

RCSteeringHandler::RCSteeringHandler()
  : pwmInSteeringPin(0), pwmLightPin(0), ledPin(0), ledBrightness(BYTE_MID),
    pwmSteeringValue(0), pwmSteeringValueMin(NO_VALUE), pwmSteeringValueMax(NO_VALUE) {
}

void RCSteeringHandler::init(byte pwmInSteeringPin, byte pwmLightPin, byte ledPin) {
  this->pwmInSteeringPin = pwmInSteeringPin;
  this->pwmLightPin = pwmLightPin;
  this->ledPin = ledPin;
}

void RCSteeringHandler::updateCalibration() {
  readChanelsValues();
  if (pwmSteeringValue < pwmSteeringValueMin || pwmSteeringValueMin == NO_VALUE) {
    pwmSteeringValueMin = pwmSteeringValue;
  }

  if (pwmSteeringValue > pwmSteeringValueMax || pwmSteeringValueMax == NO_VALUE) {
    pwmSteeringValueMax = pwmSteeringValue;
  }

  long steeringValue = map(pwmSteeringValue, pwmSteeringValueMin, pwmSteeringValueMax, BYTE_MIN, BYTE_MAX);
  steeringValue = constrain(steeringValue, BYTE_MIN, BYTE_MAX);
  analogWrite(pwmLightPin, steeringValue);  // Set hte light intenzity
  digitalWrite(ledPin, BYTE_MAX);           // Turn on selected circuit
}

void RCSteeringHandler::updateBrightnessAdjustment() {
  readChanelsValues();
  ledBrightness = map(pwmSteeringValue, pwmSteeringValueMin, pwmSteeringValueMax, BYTE_MIN, BYTE_MAX);
  ledBrightness = constrain(ledBrightness, BYTE_MIN, BYTE_MAX);
  analogWrite(pwmLightPin, ledBrightness);  // Set hte light intenzity
  digitalWrite(ledPin, BYTE_MAX);           // Turn on selected circuit
}

void RCSteeringHandler::readChanelsValues() {
  pwmSteeringValue = pulseIn(pwmInSteeringPin, HIGH);
}

void RCSteeringHandler::resetRangeLimits() {
  pwmSteeringValueMin = NO_VALUE;
  pwmSteeringValueMax = NO_VALUE;
}

void RCSteeringHandler::setRangeLimits(int pwmRangeLow, int pwmRangeHigh) {
  pwmSteeringValueMin = pwmRangeLow;
  pwmSteeringValueMax = pwmRangeHigh;
}

void RCSteeringHandler::setLedBrightness(byte ledBrightness) {
  this->ledBrightness = ledBrightness;
}

int RCSteeringHandler::getLowRangeLimit() {
  return pwmSteeringValueMin;
}

int RCSteeringHandler::getHighRangeLimit() {
  return pwmSteeringValueMax;
}

byte RCSteeringHandler::getLedBrightness() {
  return ledBrightness;
}

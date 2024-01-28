#include "LedSetup.h"
#include "AppConstants.h"
#include "LedControlBlinker.h"
#include <Arduino.h>

LedSetup::LedSetup()
  : pwmInSteeringPin(0), ledPin(0), ledBrightness(BYTE_MID),
    pwmSteeringValue(0), pwmSteeringValueMin(NO_VALUE), pwmSteeringValueMax(NO_VALUE) {
}

void LedSetup::init(byte pwmInSteeringPin, byte ledPin) {
  this->pwmInSteeringPin = pwmInSteeringPin;
  this->ledPin = ledPin;
}

void LedSetup::updateCalibration() {
  readChanelsValues();
  if (pwmSteeringValue < pwmSteeringValueMin || pwmSteeringValueMin == NO_VALUE) {
    pwmSteeringValueMin = pwmSteeringValue;
  }

  if (pwmSteeringValue > pwmSteeringValueMax || pwmSteeringValueMax == NO_VALUE) {
    pwmSteeringValueMax = pwmSteeringValue;
  }

  long buttonValue = map(pwmSteeringValue, pwmSteeringValueMin, pwmSteeringValueMax, BYTE_MIN, BYTE_MAX);
  buttonValue = constrain(buttonValue, BYTE_MIN, BYTE_MAX);
  analogWrite(ledPin, buttonValue);  // Nastavení intenzity LED
}

void LedSetup::updateBrightnessAdjustment() {
  readChanelsValues();
  ledBrightness = map(pwmSteeringValue, pwmSteeringValueMin, pwmSteeringValueMax, BYTE_MIN, BYTE_MAX);
  ledBrightness = constrain(ledBrightness, BYTE_MIN, BYTE_MAX);
  analogWrite(ledPin, ledBrightness);  // Nastavení intenzity LED
  Serial.print("SET led brightness: ");
  Serial.println(ledBrightness);
}

void LedSetup::readChanelsValues() {
  pwmSteeringValue = pulseIn(pwmInSteeringPin, HIGH);
}

void LedSetup::resetRangeLimits() {
  pwmSteeringValueMin = NO_VALUE;
  pwmSteeringValueMax = NO_VALUE;
}

void LedSetup::setRangeLimits(int pwmRangeLow, int pwmRangeHigh) {
  pwmSteeringValueMin = pwmRangeLow;
  pwmSteeringValueMax = pwmRangeHigh;
}

void LedSetup::setLedBrightness(byte ledBrightness) {
  this->ledBrightness = ledBrightness;
}

int LedSetup::getLowRangeLimit() {
  return pwmSteeringValueMin;
}

int LedSetup::getHighRangeLimit() {
  return pwmSteeringValueMax;
}

byte LedSetup::getLedBrightness() {
  return ledBrightness;
}

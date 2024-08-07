#include <Arduino.h>
#include "RCSteeringHandler.h"
#include "AppConstants.h"

RCSteeringHandler::RCSteeringHandler()
  : pwmLightPin(0), ledPin(0), ledBrightness(AppConstants::BYTE_MAX) {}

void RCSteeringHandler::init(RCChannel* rcChannel, byte pwmLightPin, byte ledPin) {
  _rcChannel = rcChannel;
  this->pwmLightPin = pwmLightPin;
  this->ledPin = ledPin;
}

void RCSteeringHandler::updateBrightnessAdjustment() {
  int value = _rcChannel->getValue();
  int min = _rcChannel->getMin();
  int max = _rcChannel->getMax();
  value = value > max ? max : value;
  value = value < min ? min : value;

  ledBrightness = map(value, min, max, AppConstants::BYTE_MIN, AppConstants::BYTE_MAX);
  ledBrightness = constrain(ledBrightness, AppConstants::BYTE_MIN, AppConstants::BYTE_MAX);
  analogWrite(pwmLightPin, ledBrightness); // Set the light intenzity
  digitalWrite(ledPin, HIGH); // Turn on selected circuit

  //describe();
}

byte RCSteeringHandler::getLedBrightness() {
  return ledBrightness;
}

void RCSteeringHandler::setLedBrightness(byte ledBrightness) {
  this->ledBrightness = ledBrightness;
}

void RCSteeringHandler::describe() {
  Serial.print("Val:");
  Serial.print(_rcChannel->getValue());
  Serial.print(", Min:");
  Serial.print(_rcChannel->getMin());
  Serial.print(", Max:");
  Serial.print(_rcChannel->getMax());
  Serial.print(", LED:");
  Serial.print(ledBrightness);
  Serial.println();
}
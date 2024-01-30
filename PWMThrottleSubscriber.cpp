#include "HardwareSerial.h"
#include "Arduino.h"
#include "PWMThrottleSubscriber.h"
#include "AppConstants.h"

PWMThrottleSubscriber::PWMThrottleSubscriber()
  : _hasValidSignal(false),
    _throttleChannelPwmPin(0),
    _analogMotorForwardPin(0),
    _analogMotorBackwardPin(0),
    _throttleReducedValue(0),
    _forwardReducedValue(0),
    _backwardReducedValue(0) {
  //
}

void PWMThrottleSubscriber::init(byte throttleChannelPwmPin, byte analogMotorForwardPin, byte analogMotorBackwardPin) {
  _throttleChannelPwmPin = throttleChannelPwmPin;
  _analogMotorForwardPin = analogMotorForwardPin;
  _analogMotorBackwardPin = analogMotorBackwardPin;
}

// Returns booleas in meaning of hasValidSignal?
bool PWMThrottleSubscriber::update() {
  unsigned long pwmRawValue = pulseIn(_throttleChannelPwmPin, HIGH);
  _hasValidSignal = pwmRawValue >= SIGNAL_VALID_LOW_VALUE_THRESHOLD && pwmRawValue <= SIGNAL_VALID_HIGH_VALUE_THRESHOLD;
  if (!_hasValidSignal) {
    return false;  // signal is invalid
  }

  int scaledDownPwmValue = map(pwmRawValue, 1000, 2000, BYTE_MIN, BYTE_MAX);
  _throttleReducedValue = constrain(scaledDownPwmValue, BYTE_MIN, BYTE_MAX);

  int forwadRawValue = analogRead(_analogMotorForwardPin);
  int forwadScaledDownValue = map(forwadRawValue, 0, 1023, BYTE_MIN, BYTE_MAX);
  _forwardReducedValue = constrain(forwadScaledDownValue, BYTE_MIN, BYTE_MAX);


  int backwadRawValue = analogRead(_analogMotorBackwardPin);
  int backwadScaledDownValue = map(backwadRawValue, 0, 1023, BYTE_MIN, BYTE_MAX);
  _backwardReducedValue = constrain(backwadScaledDownValue, BYTE_MIN, BYTE_MAX);


  // TODO, fake is here till I will connect physical DC motor via opto-isolator. I will have two of them. One for detecting forward and second for backward.
  // Fake with 5k potentiometer with 2.2K resitor at A0 + A1
  // I will map potentiometer to 0..512 range and then divide it into 2 fdake signals.
  // up to first half of potentiomenter it means motor polarization that ends with model is moving forward,
  // and from second half part of potentiometer means backward movement.
  // I need analog values to determine no move and it also gives me space to work with sounds of movements. 
  // value 863 is the measured maximum value I read at A0 of my fake potentioment circuit.
  // SEE tinkercad schematic for potentiometer
  // https://www.tinkercad.com/things/kCgubF5OM5P-shiny-snicket
  int fakeValue = map(forwadRawValue, 0, 863, BYTE_MIN, BYTE_MAX * 2);
  if (fakeValue <= 255) {
    _backwardReducedValue = 255 - fakeValue;
    _forwardReducedValue = 0;
  } else {
    _backwardReducedValue = 0;
    _forwardReducedValue = fakeValue - 255;
  }

  /*
  // debug prints
  Serial.print("T: ");
  Serial.print(_throttleReducedValue);
  Serial.print(", F: ");
  Serial.print(_forwardReducedValue);
  Serial.print(", B: ");
  Serial.println(_backwardReducedValue);
  */

  return true;  // signal is valid
}

bool PWMThrottleSubscriber::hasValidSignal() {
  return _hasValidSignal;
}

bool PWMThrottleSubscriber::isMovingForward() {
  return _forwardReducedValue > 0 && _throttleReducedValue >= FORWARD_THRESHOLD;
}

bool PWMThrottleSubscriber::isMovingBackward() {
  return _backwardReducedValue > 0 && _throttleReducedValue >= FORWARD_THRESHOLD;
}

bool PWMThrottleSubscriber::isStationary() {
  return _throttleReducedValue >= BACKWARD_THRESHOLD && _throttleReducedValue <= FORWARD_THRESHOLD && _forwardReducedValue == 0 && _backwardReducedValue == 0;
}

bool PWMThrottleSubscriber::isBreaking() {
  return (_forwardReducedValue > 0 && _throttleReducedValue < BACKWARD_THRESHOLD)
         || (_backwardReducedValue > 0 && _throttleReducedValue > FORWARD_THRESHOLD);
}

bool PWMThrottleSubscriber::isReverse() {
  return _backwardReducedValue > 0;
}

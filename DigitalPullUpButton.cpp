#include "DigitalPullUpButton.h"
#include <Arduino.h>

DigitalPullUpButton::DigitalPullUpButton(int id) : ButtonBase(id), _pin(-1) {}

void DigitalPullUpButton::init(byte buttonPin) {
  _pin = buttonPin;
  pinMode(_pin, INPUT);
  digitalWrite(buttonPin, HIGH);
}

bool DigitalPullUpButton::getIsPressed() {
  // Pro INPUT_PULLUP, tlačítko je stisknuté, když je pin LOW
  bool isPressed = digitalRead(_pin) == LOW;

  return isPressed;
}
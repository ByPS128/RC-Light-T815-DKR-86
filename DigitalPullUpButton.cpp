#include "DigitalPullUpButton.h"
#include <Arduino.h>

DigitalPullUpButton::DigitalPullUpButton(int id) : ButtonBase(id) {}

void DigitalPullUpButton::init(byte buttonPin) {
  ButtonBase::init(buttonPin);
  
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(buttonPin, HIGH);
}

bool DigitalPullUpButton::readPinState() {
  // Pro INPUT_PULLUP, tlačítko je stisknuté, když je pin LOW
  bool pressed = digitalRead(_pin) == LOW;
  return pressed;
}
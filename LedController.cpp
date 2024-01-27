#include "LedController.h"
#include "IButtonPressListener.h"
#include <Arduino.h>

void LedController::init() {
}

void LedController::onClick() {
  Serial.println("OnClick()");
}

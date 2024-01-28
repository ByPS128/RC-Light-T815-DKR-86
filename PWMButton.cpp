#include "PWMButton.h"
#include "AppConstants.h"

PWMButton::PWMButton()
  : lastPressTime(0), lastReleaseTime(0),
    pressDuration(0), isPressed(false), isLongPressed(false), pressCount(0) {
  pinMode(pwmPin, INPUT);
}

void PWMButton::init(byte pwmPin, IButtonPressListener* listener) {
  this->pwmPin = pwmPin;
  this->listener = listener;
}

void PWMButton::update() {
  unsigned long currentTime = millis();
  unsigned long pwmValue = pulseIn(pwmPin, HIGH);
  int buttonValue = map(pwmValue, 1000, 2000, BYTE_MIN, BYTE_MAX);
  buttonValue = constrain(buttonValue, BYTE_MIN, BYTE_MAX);
  bool pressedCurrent = buttonValue > BYTE_MID;

  if (pressedCurrent && !isPressed) {  // Tlačítko bylo stisknuto
    isPressed = true;
    lastPressTime = currentTime;
    pressCount++;
  } else if (!pressedCurrent && isPressed) {  // Tlačítko bylo uvolněno
    isPressed = false;
    lastReleaseTime = currentTime;
    pressDuration = currentTime - lastPressTime;

    if (pressDuration >= LONG_PRESS_DURATION) {
      isLongPressed = true;
    }

    if (pressCount == 1) {
      // Spustit časovač pro detekci jednoduchého nebo dvojitého kliknutí
      singleClickStartTime = currentTime;
    }
  }

  if ((!isPressed && pressCount == 1 && singleClickStartTime != 0 && currentTime - lastReleaseTime > DOUBLE_CLICK_TIMEOUT) || (!isPressed && pressCount > 1)) {
    // Čas pro dvojité kliknutí uplynul, zpracovat událost
    callListener();
    singleClickStartTime = 0;  // Reset časovače
    pressCount = 0;
    isLongPressed = false;
  }
}

void PWMButton::callListener() {
  if (listener) {
    listener->onClick(determineEventType());
  }
}

ButtonClickKind PWMButton::determineEventType() {
  if (isLongPressed && pressCount == 1) {
    return ButtonClickKind::LongPress;
  } else if (isLongPressed && pressCount > 1) {
    return ButtonClickKind::ClickAndLongPress;
  } else if (pressCount > 1) {
    return ButtonClickKind::DoubleClick;
  } else if (isLongPressed) {
    return ButtonClickKind::LongPress;
  } else {
    return ButtonClickKind::Click;
  }
}

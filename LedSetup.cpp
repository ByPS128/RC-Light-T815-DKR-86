#include "PwmLedBrightnessControl.h"
#include "IButtonPressListener.h"
#include "LedControlBlinker.h"
#include <Arduino.h>

LedSetup::LedSetup() : 
    pwmButtonPin(0), pwmInSteeringPin(0), ledPin(0), buttonValue(0), ledBrightness(128),
    pressedLast(false), longPress(false), lastPressTime(0), firstTriggerTime(0),
    brightnessStep(5), interval(50), triggerInterval(1000)
  {
    stateBlinker = LedControlBlinker();
  }

void LedSetup::init(IButtonPressListener* eventListener, int pwmButton, int pwmInSteering, int led) {
  this->eventListener = eventListener;
  pwmButtonPin = pwmButton;
  pwmInSteeringPin = pwmInSteering;
  ledPin = led;

  pinMode(pwmButtonPin, INPUT);
  pinMode(pwmInSteeringPin, INPUT);
  pinMode(ledPin, OUTPUT);

  readSteeringBoundsFromEprom();
  readLedBrightnessValueFromEprom();

  stateBlinker.setSignalLedPin(ledPin);
  blink5(ledBrightness);
  //blinkSOS();
}

void LedSetup::update() {
  stateBlinker.updateBlinking();
  if (stateBlinker.getIsBlinking()) return;

  readChanelsValues();
  processButton();
}

void LedSetup::readChanelsValues() {
  unsigned long pwmButtonValue = pulseIn(pwmButtonPin, HIGH); 
  buttonValue = map(pwmButtonValue, 1000, 2000, 0, 255); 
  buttonValue = constrain(buttonValue, 0, 255);
}

void LedSetup::processButton() {
  bool pressedCurrent = buttonValue > 128;
  if (pressedCurrent && !pressedLast) {
    // button is pressed
    Serial.println("click down");
    firstTriggerTime = millis();  // Uložení času stisku tlačítka
    lastPressTime = millis();  // Uložení času stisku tlačítka
    isDblClick = millis() < dblClickInterval + dblClickTime;
    dblClickTime = millis();
  }

  if (pressedCurrent) {
    if (!stateBlinker.getIsBlinking()) {
      analogWrite(ledPin, ledBrightness); // Nastavení intenzity LED
    }

    // Pokud je tlačítko stisknuto, postupně zvyšujeme nebo snižujeme jas
    if (millis() - lastPressTime > triggerInterval) {
      longPress = true;
      Serial.println("long press");
    }

    if (longPress /*&& millis() - lastPressTime > interval*/) {
      int pwmSteeringValue = pulseIn(pwmInSteeringPin, HIGH); 

      if (isDblClick) {
        if (!isDblClickPassed) {
          isDblClickPassed = true;
          pwmSteeringValueMin = pwmSteeringValue;
          pwmSteeringValueMax = pwmSteeringValue;
          Serial.println("RESET");
          blink3();
          return;
        }

        if (pwmSteeringValue < pwmSteeringValueMin) {
          pwmSteeringValueMin = pwmSteeringValue;
        }

        if (pwmSteeringValue > pwmSteeringValueMax) {
          pwmSteeringValueMax = pwmSteeringValue;
        }

        Serial.print("Min-Max: ");
        Serial.print(pwmSteeringValueMin);
        Serial.print(" - ");
        Serial.print(pwmSteeringValueMax);
        Serial.print(", ");
      } else {
        if (!isLongPressPassed) {
          isLongPressPassed = true;
          blink2();
          return;
        }
      }

      ledBrightness = map(pwmSteeringValue, pwmSteeringValueMin, pwmSteeringValueMax, 1, 255); 
      ledBrightness = constrain(ledBrightness, 0, 255);

      Serial.print("steering: ");
      Serial.print(pwmSteeringValue);
      Serial.print(", led: ");
      Serial.println(ledBrightness);

      // ledBrightness += brightnessStep;
      // if (ledBrightness > 255 || ledBrightness < 0) { 
      //   brightnessStep *= -1;
      //   ledBrightness = constrain(ledBrightness, 0, 255);
      // }
      lastPressTime = millis();
    }
  }

  if (!pressedCurrent && pressedLast != pressedCurrent) {
    // button is released
    // Vyhodnotím, jestl isjem tlačítkem ativoval nějakou programovací funkci:
    // Uložím min/max hranice hodnot volantu
    if (longPress && isDblClickPassed) writeSteeringBoundsToEprom();
    // Uložím zvolený jas ledky
    if (longPress && !isDblClickPassed) writeLedBrightnessValueToEprom();

    Serial.println("click up");

    if (eventListener && !longPress && !isDblClickPassed)
    {
      eventListener->onClick();
    }

    // reset stavů
    longPress = false;
    isDblClickPassed = false;
    isLongPressPassed = false;
    analogWrite(ledPin, 0);
  }

  pressedLast = pressedCurrent;
}

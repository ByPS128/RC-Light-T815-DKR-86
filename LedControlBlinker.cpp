#include "LedControlBlinker.h"
#include "AppConstants.h"

LedControlBlinker::LedControlBlinker()
  : _ledPinsArrayLength(0), currentStep(0), lastBlinkTime(0), isBlinkingActive(false), sequenceLength(0), infiniteLoopEnabled(false) {
}

void LedControlBlinker::init(ILedBlinkerStopListener* listener, byte pwmLedPin) {
  _listener = listener;
  _ledPinsArray[0] = pwmLedPin;
  _ledPinsArrayLength = 1;
}

void LedControlBlinker::init(ILedBlinkerStopListener* listener, byte ledPin1, byte ledPin2, byte ledPin3, byte ledPin4, byte ledPin5, byte ledPin6) {
  _listener = listener;
  _ledPinsArray[0] = ledPin1;
  _ledPinsArray[1] = ledPin2;
  _ledPinsArray[2] = ledPin3;
  _ledPinsArray[3] = ledPin4;
  _ledPinsArray[4] = ledPin5;
  _ledPinsArray[5] = ledPin6;
  _ledPinsArrayLength = 6;
}

void LedControlBlinker::startBlinking(unsigned int count, byte onBrightness, unsigned long onDuration, byte offBrightness, unsigned long offDuration, byte darkBrightness, unsigned long darkTAfterBlinkDuration) {
  for (unsigned int i = 0; i < count; i++) {
    if (i < MAX_SEQUENCE_LENGTH / 2) {
      blinkSequence[i * 2] = { onDuration, onBrightness };
      blinkSequence[i * 2 + 1] = { offDuration, offBrightness };
    }
  }

  if (darkTAfterBlinkDuration > 0) {
    blinkSequence[count * 2] = { darkTAfterBlinkDuration, darkBrightness };
  }

  startBlinkingSequence(blinkSequence, count * 2);
}

void LedControlBlinker::startBlinkingSequence(const BlinkStep sequence[], unsigned int length) {
  sequenceLength = length > MAX_SEQUENCE_LENGTH ? MAX_SEQUENCE_LENGTH : length;
  memcpy(blinkSequence, sequence, sequenceLength * sizeof(BlinkStep));
  currentStep = 0;
  isBlinkingActive = true;
  lastBlinkTime = millis();
}

bool LedControlBlinker::getIsBlinking() {
  return isBlinkingActive;
}

void LedControlBlinker::enableInfiniteLoop() {
  infiniteLoopEnabled = true;
}

void LedControlBlinker::disableInfiniteLoop() {
  infiniteLoopEnabled = false;
}

void LedControlBlinker::stop() {
  isBlinkingActive = false;
  onAnimationEnd();
}

bool LedControlBlinker::updateBlinking() {
  if (infiniteLoopEnabled && !isBlinkingActive) {
    isBlinkingActive = true;
  }

  if (!isBlinkingActive) return false;

  unsigned long currentMillis = millis();
  if (currentStep < sequenceLength) {
    const BlinkStep& step = blinkSequence[currentStep];
    // Přepnutí stavu LED
    byte ledBrightness = step.ledBrightness;
    updateLed(ledBrightness);

    if (currentMillis - lastBlinkTime >= step.duration) {
      // Přechod na další krok v sekvenci
      lastBlinkTime = currentMillis;
      currentStep++;

      // Pokud je dosaženo konce sekvence, nastaví se temné období
      if (currentStep >= sequenceLength) {
        if (infiniteLoopEnabled) {
          currentStep = 0;
          return isBlinkingActive;
        }
        ledBrightness = 0;
        isBlinkingActive = false;
        updateLed(ledBrightness);
        onAnimationEnd();
      }
    }
  }

  return isBlinkingActive;
}

void LedControlBlinker::updateLed(byte ledBrightness) {
  if (_ledPinsArrayLength == 1) {
    analogWrite(_ledPinsArray[0], ledBrightness);
    return;
  }

  for (byte i = 0; i < _ledPinsArrayLength; i++) {
    analogWrite(_ledPinsArray[i], ledBrightness);
  }
}

void LedControlBlinker::onAnimationEnd() {
  if (!_listener) {
    return;
  }

  _listener->onLedBlinkerAnimationStop(this);
}

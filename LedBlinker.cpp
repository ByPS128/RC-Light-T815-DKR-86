#include "LedBlinker.h"
#include "AppConstants.h"

LedBlinker::LedBlinker()
  : _ledPinsArrayLength(0), currentStep(0), lastBlinkTime(0), isBlinkingActive(false), sequenceLength(0), infiniteLoopEnabled(false) {
}

void LedBlinker::init(byte pwmLedPin) {
  _ledPinsArray[0] = pwmLedPin;
  _ledPinsArrayLength = 1;
}

void LedBlinker::init(byte ledPin1, byte ledPin2, byte ledPin3, byte ledPin4, byte ledPin5, byte ledPin6) {
  _ledPinsArray[0] = ledPin1;
  _ledPinsArray[1] = ledPin2;
  _ledPinsArray[2] = ledPin3;
  _ledPinsArray[3] = ledPin4;
  _ledPinsArray[4] = ledPin5;
  _ledPinsArray[5] = ledPin6;
  _ledPinsArrayLength = 6;
}

void LedBlinker::registerSubscriber(ILedBlinkerSubscriber* subscriber) {
  if (subscriber) {
    _subscribers.add(subscriber);
  }
}

void LedBlinker::unregisterSubscriber(ILedBlinkerSubscriber* subscriber) {
  for (int i = 0; i < _subscribers.size(); i++) {
    if (_subscribers.get(i) == subscriber) {
      _subscribers.remove(i);
      break;
    }
  }
}

void LedBlinker::startBlinking(unsigned int count, byte onBrightness, unsigned long onDuration, byte offBrightness, unsigned long offDuration, byte darkBrightness, unsigned long darkAfterBlinkDuration) {
  for (unsigned int i = 0; i < count; i++) {
    if (i < MAX_SEQUENCE_LENGTH / 2) {
      blinkSequence[i * 2] = { onDuration, onBrightness };
      blinkSequence[i * 2 + 1] = { offDuration, offBrightness };
    }
  }

  if (darkAfterBlinkDuration > 0) {
    blinkSequence[count * 2] = { darkAfterBlinkDuration, darkBrightness };
  }

  startBlinkingSequence(blinkSequence, count * 2);
}

void LedBlinker::startBlinkingSequence(const BlinkStep sequence[], unsigned int length) {
  sequenceLength = length > MAX_SEQUENCE_LENGTH ? MAX_SEQUENCE_LENGTH : length;
  memcpy(blinkSequence, sequence, sequenceLength * sizeof(BlinkStep));
  currentStep = 0;
  isBlinkingActive = true;
  lastBlinkTime = millis();
}

bool LedBlinker::getIsBlinking() {
  return isBlinkingActive;
}

void LedBlinker::enableInfiniteLoop() {
  infiniteLoopEnabled = true;
}

void LedBlinker::disableInfiniteLoop() {
  infiniteLoopEnabled = false;
}

void LedBlinker::stop() {
  isBlinkingActive = false;
  onAnimationEnd();
}

bool LedBlinker::updateBlinking() {
  if (infiniteLoopEnabled && !isBlinkingActive) {
    isBlinkingActive = true;
  }

  if (!isBlinkingActive) return false;

  unsigned long currentMillis = millis();
  if (currentStep < sequenceLength) {
    const BlinkStep& step = blinkSequence[currentStep];
    // LED status switching
    byte ledBrightness = step.ledBrightness;
    updateLed(ledBrightness);

    if (currentMillis - lastBlinkTime >= step.duration) {
      // Transition to the next step in the sequence
      lastBlinkTime = currentMillis;
      currentStep++;

      // When the end of the sequence is reached, I decide whether to repeat the animation or call the subscribers
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

void LedBlinker::updateLed(byte ledBrightness) {
  if (_ledPinsArrayLength == 1) {
    analogWrite(_ledPinsArray[0], ledBrightness);
    return;
  }

  for (byte i = 0; i < _ledPinsArrayLength; i++) {
    analogWrite(_ledPinsArray[i], ledBrightness);
  }
}

void LedBlinker::onAnimationEnd() {
for (int i = 0; i < _subscribers.size(); i++) {
    ILedBlinkerSubscriber* subscriber = _subscribers.get(i);
    if (subscriber) {
      subscriber->onLedBlinkerAnimationStop(this);
    }
  }
}

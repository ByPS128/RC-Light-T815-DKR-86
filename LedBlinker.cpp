#include "LedBlinker.h"
#include "AppConstants.h"

LedBlinker::LedBlinker()
  : _pwmPin(AppConstants::BYTE_MAX), _ledPinsArrayLength(0), currentStep(0), lastBlinkTime(0), isBlinkingActive(false), sequenceLength(0), infiniteLoopEnabled(false) {
}

void LedBlinker::init(uint8_t pwmPin, uint8_t ledPin1) {
  _pwmPin = pwmPin;
  _ledPinsArray[0] = ledPin1;
  _ledPinsArrayLength = 1;
}

void LedBlinker::init(uint8_t pwmPin, uint8_t ledPin1, uint8_t ledPin2, uint8_t ledPin3, uint8_t ledPin4, uint8_t ledPin5, uint8_t ledPin6, uint8_t ledPin7) {
  _pwmPin = pwmPin;
  _ledPinsArray[0] = ledPin1;
  _ledPinsArray[1] = ledPin2;
  _ledPinsArray[2] = ledPin3;
  _ledPinsArray[3] = ledPin4;
  _ledPinsArray[4] = ledPin5;
  _ledPinsArray[5] = ledPin6;
  _ledPinsArray[6] = ledPin7;
  _ledPinsArrayLength = 7;
}

void LedBlinker::registerSubscriber(ILedBlinkerSubscriber* subscriber) {
  if (subscriber) {
    _subscribers.add(subscriber);
  }
}

void LedBlinker::unregisterSubscriber(ILedBlinkerSubscriber* subscriber) {
  for (uint8_t i = 0; i < _subscribers.size(); i++) {
    if (_subscribers.get(i) == subscriber) {
      _subscribers.remove(i);
      break;
    }
  }
}

void LedBlinker::setupBlinkingSequence(uint8_t count, uint8_t onBrightness, uint16_t onDuration, uint8_t offBrightness, uint16_t offDuration, uint8_t darkBrightness, uint16_t darkAfterBlinkDuration) {
  for (uint8_t i = 0; i < count; i++) {
    if (i < MAX_SEQUENCE_LENGTH / 2) {
      blinkSequence[i * 2] = { onDuration, onBrightness };
      blinkSequence[i * 2 + 1] = { offDuration, offBrightness };
    }
  }

  if (darkAfterBlinkDuration > 0) {
    blinkSequence[count * 2] = { darkAfterBlinkDuration, darkBrightness };
  }

  setupBlinkingSequence(blinkSequence, count * 2);
}

void LedBlinker::setupBlinkingSequence(const BlinkStep sequence[], uint8_t length) {
  sequenceLength = length > MAX_SEQUENCE_LENGTH ? MAX_SEQUENCE_LENGTH : length;
  memcpy(blinkSequence, sequence, sequenceLength * sizeof(BlinkStep));
  currentStep = 0;
  lastBlinkTime = 0;
  isBlinkingActive = false;
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

void LedBlinker::start() {
  currentStep = 0;
  lastBlinkTime = millis();
  isBlinkingActive = true;
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
    uint8_t ledBrightness = step.ledBrightness;
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

void LedBlinker::updateLed(uint8_t ledBrightness) {
  uint8_t ledState = ledBrightness == 0 ? LOW : HIGH;
  if (_pwmPin != AppConstants::BYTE_MAX) {
    analogWrite(_pwmPin, ledBrightness);
  }

  for (uint8_t i = 0; i < _ledPinsArrayLength; i++) {
    digitalWrite(_ledPinsArray[i], ledState);
  }
}

void LedBlinker::onAnimationEnd() {
  for (uint8_t i = 0; i < _subscribers.size(); i++) {
    ILedBlinkerSubscriber* subscriber = _subscribers.get(i);
    if (subscriber) {
      subscriber->onLedBlinkerAnimationStop(this);
    }
  }
}

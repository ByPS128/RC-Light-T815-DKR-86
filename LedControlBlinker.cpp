#include "LedControlBlinker.h"

LedControlBlinker::LedControlBlinker() : 
  signalPwmLedPin(0), currentStep(0), lastBlinkTime(0), isBlinkingActive(false), sequenceLength(0), ledBrightness(0) {
}

void LedControlBlinker::setSignalLedPin(unsigned short signalLedPin) {
  signalPwmLedPin = signalLedPin;
}

void LedControlBlinker::startBlinking(int count, byte onBrightness, unsigned long onDuration, byte offBrightness, unsigned long offDuration, byte darkBrightness, unsigned long darkTAfterBlinkDuration) {
  for (int i = 0; i < count; i++) {
    if (i < maxSequenceLength / 2) {
      blinkSequence[i * 2] = {onDuration, onBrightness};
      blinkSequence[i * 2 + 1] = {offDuration, offBrightness};
    }
  }
  
  if (darkTAfterBlinkDuration > 0) {
    blinkSequence[count * 2] = {darkTAfterBlinkDuration, darkBrightness};
  }

  startBlinkingSequence(blinkSequence, count * 2);
}

void LedControlBlinker::startBlinkingSequence(const BlinkStep sequence[], unsigned int length) {
  sequenceLength = length > maxSequenceLength ? maxSequenceLength : length;
  memcpy(blinkSequence, sequence, sequenceLength * sizeof(BlinkStep));
  currentStep = 0;
  isBlinkingActive = true;
  lastBlinkTime = millis();
}

bool LedControlBlinker::getIsBlinking() {
  return isBlinkingActive;
}

void LedControlBlinker::updateBlinking() {
  if (!isBlinkingActive) return;

  unsigned long currentMillis = millis();
  if (currentStep < sequenceLength) {
    const BlinkStep& step = blinkSequence[currentStep];
    // Přepnutí stavu LED
    ledBrightness = step.ledBrightness;
    updateLed();

    if (currentMillis - lastBlinkTime >= step.duration) {
      // Přechod na další krok v sekvenci
      lastBlinkTime = currentMillis;
      currentStep++;

      // Pokud je dosaženo konce sekvence, nastaví se temné období
      if (currentStep >= sequenceLength) {
        ledBrightness = 0;
        isBlinkingActive = false;
        updateLed();
      }
    }
  }
}

void LedControlBlinker::updateLed() {
  analogWrite(signalPwmLedPin, ledBrightness);
}

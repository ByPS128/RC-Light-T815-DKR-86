#pragma once

#include <Arduino.h>

class LedControlBlinker {
public:
  struct BlinkStep {
    unsigned long duration;
    byte ledBrightness;
  };

  LedControlBlinker();
  void init(byte pwmLedPin);
  bool updateBlinking();
  void startBlinking(int count, byte onBrightness, unsigned long onDuration, byte offBrightness, unsigned long offDuration, byte darkBrightness, unsigned long darkTAfterBlinkDuration);
  void startBlinkingSequence(const BlinkStep sequence[], unsigned int sequenceLength);
  bool getIsBlinking();

private:
  void updateLed(byte ledBrightness);

private:
  byte pwmLedPin;
  static const unsigned int MAX_SEQUENCE_LENGTH = 32; // Limiting the length of the array to save resources. Note that the value must be an even number.
  BlinkStep blinkSequence[MAX_SEQUENCE_LENGTH];
  unsigned int sequenceLength;
  unsigned int currentStep;
  unsigned long lastBlinkTime;
  bool isBlinkingActive;
};

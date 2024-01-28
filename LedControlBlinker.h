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
  void updateLed();

private:
  byte pwmLedPin;
  static const unsigned int maxSequenceLength = 32;  // Příklad maximální délky
  BlinkStep blinkSequence[maxSequenceLength];
  unsigned int sequenceLength;
  unsigned int currentStep;
  unsigned long lastBlinkTime;
  bool isBlinkingActive;
  byte ledBrightness;
};

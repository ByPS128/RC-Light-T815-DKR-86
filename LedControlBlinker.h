#pragma once

#include <Arduino.h>

class LedControlBlinker; // Forward declaration

class ILedBlinkerStopListener {
public:
  virtual void onLedBlinkerAnimationStop(LedControlBlinker* instance) = 0;
};

class LedControlBlinker {
public:
  struct BlinkStep {
    unsigned long duration;
    byte ledBrightness;
  };

  LedControlBlinker();
  void init(ILedBlinkerStopListener* listener, byte pwmLedPin);
  void init(ILedBlinkerStopListener* listener, byte ledPin1, byte ledPin2, byte ledPin3, byte ledPin4, byte ledPin5, byte ledPin6);
  bool updateBlinking();
  void startBlinking(unsigned int count, byte onBrightness, unsigned long onDuration, byte offBrightness, unsigned long offDuration, byte darkBrightness, unsigned long darkTAfterBlinkDuration);
  void startBlinkingSequence(const BlinkStep sequence[], unsigned int sequenceLength);
  bool getIsBlinking();
  void enableInfiniteLoop();
  void disableInfiniteLoop();
  void stop();

private:
  void updateLed(byte ledBrightness);
  void onAnimationEnd();

private:
  static const byte MAX_SEQUENCE_LENGTH = 32;  // Limiting the length of the array to save resources. Note that the value must be an even number.
  static const byte MAX_PIN_LENGTH = 12;

private:
  ILedBlinkerStopListener* _listener;
  byte _ledPinsArrayLength;
  unsigned int currentStep;
  unsigned long lastBlinkTime;
  bool isBlinkingActive;
  unsigned int sequenceLength;
  bool infiniteLoopEnabled;
  byte _ledPinsArray[MAX_PIN_LENGTH];
  BlinkStep blinkSequence[MAX_SEQUENCE_LENGTH];
};

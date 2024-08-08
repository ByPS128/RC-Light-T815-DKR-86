#pragma once

#include <Arduino.h>
#include <LinkedList.h>

class LedBlinker;  // Forward declaration

class ILedBlinkerSubscriber {
public:
  virtual void onLedBlinkerAnimationStop(LedBlinker* instance) = 0;
};

class LedBlinker {
public:
  struct BlinkStep {
    uint16_t duration;
    uint8_t ledBrightness;
  };

  LedBlinker();
  void init(uint8_t pwmPin, uint8_t ledPin1);
  void init(uint8_t pwmPin, uint8_t ledPin1, uint8_t ledPin2, uint8_t ledPin3, uint8_t ledPin4, uint8_t ledPin5, uint8_t ledPin6, uint8_t ledPin7);
  void registerSubscriber(ILedBlinkerSubscriber* subscriber);
  void unregisterSubscriber(ILedBlinkerSubscriber* subscriber);
  bool updateBlinking();
  void setupBlinkingSequence(uint8_t count, uint8_t onBrightness, uint16_t onDuration, uint8_t offBrightness, uint16_t offDuration, uint8_t darkBrightness, uint16_t darkAfterBlinkDuration);
  void setupBlinkingSequence(const BlinkStep sequence[], uint8_t sequenceLength);
  bool getIsBlinking();
  void enableInfiniteLoop();
  void disableInfiniteLoop();
  void start();
  void stop();

private:
  void updateLed(uint8_t ledBrightness);
  void onAnimationEnd();

private:
  static const uint8_t MAX_SEQUENCE_LENGTH = 32;  // Limiting the length of the array to save resources. Note that the value must be an even number.
  static const uint8_t MAX_PIN_LENGTH = 12;

private:
  LinkedList<ILedBlinkerSubscriber*> _subscribers;
  uint8_t _pwmPin;
  uint8_t _ledPinsArrayLength;
  uint8_t currentStep;
  unsigned long lastBlinkTime;
  bool isBlinkingActive;
  uint8_t sequenceLength;
  bool infiniteLoopEnabled;
  uint8_t _ledPinsArray[MAX_PIN_LENGTH];
  BlinkStep blinkSequence[MAX_SEQUENCE_LENGTH];
};

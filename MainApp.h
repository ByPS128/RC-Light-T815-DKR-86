#pragma once

#include <Arduino.h>
#include "LedBlinker.h"
#include "RcPwmButton.h"
#include "DigitalPullUpButton.h"
#include "RCThrottleHandler.h"
#include "RCSteeringHandler.h"
#include "LightsController.h"
#include "RCChannel.h"
#include "CalibrationManager.h"
#include "AppConstants.h"
#include "SignalValidator.h"

enum class ProgrammingModes {
  None,
  BrightnessAdjustment
};

class MainApp : public IButtonEventSubscriber, ILedBlinkerSubscriber {
private:
  RcPwmButton rcButton;
  DigitalPullUpButton calibrationButton;
  ProgrammingModes currentMode;
  uint8_t ledBrightness;
  RCSteeringHandler steeringHandler;
  RCThrottleHandler throttleHandler;
  LedBlinker ledBlinker;
  LedBlinker noSignalBlinker;
  LedBlinker notCalibratedBlinker;
  LightsController lightsController;

  RCChannel* channels[AppConstants::CHANNEL_COUNT];
  CalibrationManager* calibrationManager;
  SignalValidator* signalValidator;

public:
  MainApp();
  void init();
  void update();
  void onButtonClick(int buttonId, ButtonClickType clickKind) override;
  void onLedBlinkerAnimationStop(LedBlinker* instance) override;

private:
  void updateRcChannels();
  void setupPins();
  // composed I/O ops
  void readLedBrightnessValueFromEprom();
  void writeLedBrightnessValueToEprom();
  // button events
  void onRcPwmButtonClick(ButtonClickType clickKind);
  void onCalibrationButtonClick(ButtonClickType clickKind);
  // Animation macros
  void blinkApplicationReady();
  void blinkStartCalibrating();
  void blinkStartBrightnessAdjustment();
  void blinkWriteOK();
  void setupNotCalibrated();
  void setupNoSignal();
  //
  const __FlashStringHelper* buttonClickTypeToString(ButtonClickType kind);
  void describeRcChannel(int channelIndex, bool performLineFeed);
};

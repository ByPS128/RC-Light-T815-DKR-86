#pragma once

#include <Arduino.h>
#include "LedBlinker.h"
#include "RcPwmButton.h"
#include "RCThrottleHandler.h"
#include "RCSteeringHandler.h"
#include "LightsController.h"

enum class ProgrammingModes {
  None,
  Calibrating,
  BrightnessAdjustment
};

class MainApp : public IButtonEventSubscriber, ILedBlinkerSubscriber {
private:
  ProgrammingModes currentMode;
  RCSteeringHandler steeringHandler;
  RcPwmButton buttonHandler;
  RCThrottleHandler throttleHandler;
  LedBlinker ledBlinker;
  LedBlinker noSignalBlinker;
  LightsController lightsController;
  byte ledBrightness;
  int pwmSteeringValueMin;
  int pwmSteeringValueMax;

public:
  MainApp();
  void init();
  void update();
  void onButtonClick(int buttonId, ButtonClickType clickKind) override;
  void onLedBlinkerAnimationStop(LedBlinker* instance) override;

private:
  void setupPins();
  // atomic emprom ops for WORD.
  int EEPROMReadInt(int address);
  void EEPROMWriteInt(int address, int value);
  // composed I/O ops
  void readSteeringBoundsFromEprom();
  void writeSteeringBoundsToEprom();
  void readLedBrightnessValueFromEprom();
  void writeLedBrightnessValueToEprom();
  // button events
  void onRcPwmButtonClick(ButtonClickType clickKind);
  // Animation macros
  void blinkApplicationReady(byte useBrightness);
  void blinkStartCalibrating();
  void blinkStartBrightnessAdjustment();
  void blinkWriteOK();
  void setupSOS();
  void setupNoSignal();
  //
  String buttonClickTypeToString(ButtonClickType kind);
};

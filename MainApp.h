#pragma once

#include <Arduino.h>
#include "LedBlinker.h"
#include "RCButtonClickHandler.h"
#include "RCThrottleHandler.h"
#include "RCSteeringHandler.h"
#include "LightsController.h"

enum class ProgrammingModes {
  None,
  Calibrating,
  BrightnessAdjustment
};

class MainApp : public IRCButtonClickSubscriber, ILedBlinkerSubscriber {
private:
  ProgrammingModes currentMode;
  RCSteeringHandler steeringHandler;
  RCButtonClickHandler buttonHandler;
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
  void onButtonClick(RCButtonClickKind clickKind) override;
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
  // Animation macros
  void blinkApplicationReady(byte useBrightness);
  void blinkStartCalibrating();
  void blinkStartBrightnessAdjustment();
  void blinkWriteOK();
  void setupSOS();
  void setupNoSignal();
  //
  String buttonClickKindToString(RCButtonClickKind kind);
};

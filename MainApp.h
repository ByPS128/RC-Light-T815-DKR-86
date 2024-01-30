#pragma once

#include <Arduino.h>
#include "LedControlBlinker.h"
#include "PWMButton.h"
#include "PWMThrottleSubscriber.h"
#include "LedSetup.h"
#include "LightsController.h"

enum class ProgrammingModes {
  None,
  Calibrating,
  BrightnessAdjustment
};

class MainApp : public IButtonPressListener, ILedBlinkerStopListener {
private:
  ProgrammingModes currentMode;
  LedSetup ledSetup;
  PWMButton pwmButton;
  LedControlBlinker ledBlinker;
  LedControlBlinker noSignalBlinker;
  LightsController lightsController;
  PWMThrottleSubscriber throttleSubsriber;
  byte ledBrightness;
  int pwmSteeringValueMin;
  int pwmSteeringValueMax;

public:
  MainApp();
  void init();
  void update();
  void onClick(ButtonClickKind clickKind) override;
  void onLedBlinkerAnimationStop(LedControlBlinker* instance) override;

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
  String buttonClickKindToString(ButtonClickKind kind);
};

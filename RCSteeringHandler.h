#pragma once

#include <Arduino.h>
#include "LedBlinker.h"

class RCSteeringHandler {
private:
  const long NO_VALUE = -99999;

private:
  byte pwmInSteeringPin;
  byte pwmLightPin;
  byte ledPin;
  byte ledBrightness;
  long pwmSteeringValue;
  long pwmSteeringValueMin = NO_VALUE;
  long pwmSteeringValueMax = NO_VALUE;

public:
  RCSteeringHandler();
  /**
   * Initializes the controller with specific hardware pins.
   * 
   * @param pwmInSteeringPin Pin used for PWM input of RC receiver steering channel.
   *                          This pin reads the PWM signal to determine steering commands.
   * @param pwmLightPin      Pin used for PWM output to control lights intensity.
   *                          This pin modulates separated circuit which powwers LED diods.
   * @param ledPin           Pin connected to a LED.
   *                          This pin is used to set ON/OFF led that is connected to circuit with pwmLightPin modulation.
   *                          This is designed to be turned ON or OFF via NPN transistor whish base is controled by this ledPin.
   */
  void init(byte pwmInSteeringPin, byte pwmLightPin, byte ledPin);
  void updateCalibration();
  void updateBrightnessAdjustment();
  void setRangeLimits(int pwmRangeLow, int pwmRangeHigh);
  void resetRangeLimits();
  void setLedBrightness(byte ledBrightness);
  int getLowRangeLimit();
  int getHighRangeLimit();
  byte getLedBrightness();

private:
  void readChanelsValues();
  void processButton();
};

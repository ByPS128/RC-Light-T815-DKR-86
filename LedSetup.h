#ifndef LedSetup_h
#define LedSetup_h

#include <Arduino.h>
#include "LedControlBlinker.h"
#include "IButtonPressListener.h"

class LedSetup {
  private:
    int pwmButtonPin;
    int pwmInSteeringPin;
    int ledPin;
    int buttonValue;
    int ledBrightness;
    bool pressedLast;
    bool longPress;
    unsigned long lastPressTime;
    unsigned long firstTriggerTime;
    int brightnessStep;
    unsigned long interval;
    unsigned long triggerInterval;
    unsigned long dblClickTime = 0;
    bool isDblClick = false;
    int pwmSteeringValueMin = -99999;
    int pwmSteeringValueMax = -99999;
    bool isDblClickPassed = false;
    bool isLongPressPassed = false;
    LedControlBlinker stateBlinker;
    IButtonPressListener* eventListener;

    const int dblClickInterval = 200;
    const int LED_BRIGHTNESS_VALUE_ADDRESS = 0x0000;
    const int STEERING_LOW_PWM_VALUE_ADDRESS = 0x0001;
    const int STEERING_HIGH_PWM_VALUE_ADDRESS = 0x0003;

  public:
    LedSetup();
    void init(IButtonPressListener* eventListener, int pwmButton, int pwmInSteering, int led);
    void update();

  private:  
    void readChanelsValues();
    void processButton();
    void blink5(byte ledBrightness);
    void blink3();
    void blink2();
    void blinkSOS();
    int EEPROMReadInt(int address);
    void EEPROMWriteInt(int address, int value);
    void readSteeringBoundsFromEprom();
    void writeSteeringBoundsToEprom();
    void readLedBrightnessValueFromEprom();
    void writeLedBrightnessValueToEprom();
};

#endif

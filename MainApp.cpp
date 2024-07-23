#include <Arduino.h>
#include <EEPROM.h>
#include "MainApp.h"
#include "AppConstants.h"

MainApp::MainApp()
  : currentMode(ProgrammingModes::None), ledBrightness(0), pwmSteeringValueMin(0), pwmSteeringValueMax(0) {
}

void MainApp::init() {
  Serial.begin(9600);

  setupPins();

  buttonHandler.init(PIN_PWM_BUTTON);
  buttonHandler.registerSubscriber(this);
  throttleHandler.init(PIN_PWM_THROTTLE, PIN_DIGI_MOTOR_BACKWARD);
  ledBlinker.init(PIN_SIGNAL_LED);
  ledBlinker.registerSubscriber(this);
  noSignalBlinker.init(PIN_DIGI_LIGHT_MODE_1_LED, PIN_DIGI_LIGHT_MODE_2_LED, PIN_DIGI_LIGHT_MODE_3_LED, PIN_DIGI_OUTER_BRAKE_LED, PIN_DIGI_OUTER_BRAKE_MODE, PIN_DIGI_INNER_BRAKE_LED, PIN_DIGI_REVERSE_LED);
  noSignalBlinker.registerSubscriber(this);
  setupNoSignal();
  steeringHandler.init(PIN_PWM_STEERING, PIN_PWM_LIGHT_FRONT_LED, PIN_SIGNAL_LED);

  readSteeringBoundsFromEprom();
  readLedBrightnessValueFromEprom();

  lightsController.init(LightsController::MODE_NONE,
                        ledBrightness, PIN_PWM_LIGHT_FRONT_LED,
                        PIN_DIGI_LIGHT_MODE_1_LED, PIN_DIGI_LIGHT_MODE_2_LED, PIN_DIGI_LIGHT_MODE_3_LED,
                        PIN_DIGI_OUTER_BRAKE_LED, PIN_DIGI_OUTER_BRAKE_MODE,
                        PIN_DIGI_INNER_BRAKE_LED, PIN_DIGI_REVERSE_LED);

  blinkApplicationReady(ledBrightness);
}

void MainApp::update() {
  // If the LED animation is running, I don't perform any logic.
  if (ledBlinker.updateBlinking()) {
    // LED Animation is in progress.
    // Small pause to reduce CPU load
    delay(LOOP_DELAY);
    return;
  }

  bool hasValidSignal = buttonHandler.update();
  hasValidSignal &= throttleHandler.update();
  if (!hasValidSignal) {
    noSignalBlinker.updateBlinking();
    delay(LOOP_DELAY);
    return;
  }

  if (noSignalBlinker.getIsBlinking()) {
    noSignalBlinker.stop();
    lightsController.setLightsPinsByCurrentMode();
    Serial.println("Signal restored");
  }

  bool stateChanged = lightsController.setReverse(throttleHandler.isReverse());
  stateChanged |= lightsController.setBreaking(throttleHandler.isBreaking());
  if (stateChanged) {
    lightsController.setLightsPinsByCurrentMode();
  }

  // I will perform the actions depending on the programming mode.
  switch (currentMode) {
    case ProgrammingModes::None:
      // I'm not in programming mode
      break;
    case ProgrammingModes::Calibrating:
      // I'm in calibration mode
      steeringHandler.updateCalibration();
      break;
    case ProgrammingModes::BrightnessAdjustment:
      // I'm in LED brightness adjustment mode
      steeringHandler.updateBrightnessAdjustment();
      break;
  }

  delay(LOOP_DELAY);  // Small pause to reduce CPU load
}

void MainApp::onButtonClick(RCButtonClickKind clickKind) {
  Serial.print("OnClick -> ");
  Serial.println(buttonClickKindToString(clickKind));

  // Standard single click when no programming mode is in progress.
  if (clickKind == RCButtonClickKind::Click && currentMode == ProgrammingModes::None) {
    lightsController.turnToNextLightMode();
    return;
  }

  // Standard double click when no programming mode is in progress.
  if (clickKind == RCButtonClickKind::DoubleClick && currentMode == ProgrammingModes::None) {
    lightsController.turnMaximumLights();
    return;
  }

  // Single click in calibrating programming mode,
  // it will end the mdoe and store calibrated values.
  // It also turn mode to standart mode -> none.
  if (clickKind == RCButtonClickKind::Click && currentMode == ProgrammingModes::Calibrating) {
    currentMode = ProgrammingModes::None;
    writeSteeringBoundsToEprom();
    blinkWriteOK();
    return;
  }

  // Single click in brightness adjustment programming mode,
  // it will end the mdoe and store calibrated values.
  // It also turn mode to standart mode -> none.
  if (clickKind == RCButtonClickKind::Click && currentMode == ProgrammingModes::BrightnessAdjustment) {
    currentMode = ProgrammingModes::None;
    writeLedBrightnessValueToEprom();
    lightsController.setLedBirigthness(ledBrightness);
    blinkWriteOK();
    return;
  }

  if (clickKind == RCButtonClickKind::LongPress) {
    currentMode = ProgrammingModes::BrightnessAdjustment;
    blinkStartBrightnessAdjustment();
    return;
  }

  if (clickKind == RCButtonClickKind::ClickAndLongPress) {
    currentMode = ProgrammingModes::Calibrating;
    blinkStartCalibrating();
    steeringHandler.resetRangeLimits();
    return;
  }
}

void MainApp::onLedBlinkerAnimationStop(LedBlinker* instance) {
  (void)instance;
  lightsController.setLightsPinsByCurrentMode();
}

// private methods follows

void MainApp::setupPins() {
  pinMode(PIN_PWM_BUTTON, INPUT);
  pinMode(PIN_PWM_STEERING, INPUT);
  pinMode(PIN_PWM_THROTTLE, INPUT);
  pinMode(PIN_PWM_LIGHT_FRONT_LED, OUTPUT);
  pinMode(PIN_DIGI_LIGHT_MODE_1_LED, OUTPUT);
  pinMode(PIN_DIGI_LIGHT_MODE_2_LED, OUTPUT);
  pinMode(PIN_DIGI_LIGHT_MODE_3_LED, OUTPUT);
  pinMode(PIN_DIGI_OUTER_BRAKE_LED, OUTPUT);
  pinMode(PIN_DIGI_OUTER_BRAKE_MODE, OUTPUT);
  pinMode(PIN_DIGI_INNER_BRAKE_LED, OUTPUT);
  pinMode(PIN_DIGI_REVERSE_LED, OUTPUT);

  pinMode(PIN_DIGI_MOTOR_BACKWARD, INPUT);
}

int MainApp::EEPROMReadInt(int address) {
  // Reading 2 bytes from EEPROM and their connection
  byte lowByte = EEPROM.read(address);
  byte highByte = EEPROM.read(address + 1);

  return (int)lowByte + ((int)highByte << 8);
}

void MainApp::EEPROMWriteInt(int address, int value) {
  // Splitting int into 2 bytes and storing in EEPROM
  byte lowByte = value & 0xFF;
  byte highByte = (value >> 8) & 0xFF;

  EEPROM.write(address, lowByte);
  EEPROM.write(address + 1, highByte);
}

void MainApp::readSteeringBoundsFromEprom() {
  pwmSteeringValueMin = EEPROMReadInt(STEERING_LOW_PWM_VALUE_ADDRESS);
  pwmSteeringValueMax = EEPROMReadInt(STEERING_HIGH_PWM_VALUE_ADDRESS);

  steeringHandler.setRangeLimits(pwmSteeringValueMin, pwmSteeringValueMax);

  Serial.print("EPROM read bounds: ");
  Serial.print(pwmSteeringValueMin);
  Serial.print(" - ");
  Serial.println(pwmSteeringValueMax);
}

void MainApp::writeSteeringBoundsToEprom() {
  pwmSteeringValueMin = steeringHandler.getLowRangeLimit();
  pwmSteeringValueMax = steeringHandler.getHighRangeLimit();
  
  EEPROMWriteInt(STEERING_LOW_PWM_VALUE_ADDRESS, pwmSteeringValueMin);
  EEPROMWriteInt(STEERING_HIGH_PWM_VALUE_ADDRESS, pwmSteeringValueMax);
  
  Serial.print("EPROM write bounds: ");
  Serial.print(pwmSteeringValueMin);
  Serial.print(" - ");
  Serial.println(pwmSteeringValueMax);
}

void MainApp::readLedBrightnessValueFromEprom() {
  ledBrightness = EEPROM.read(LED_BRIGHTNESS_VALUE_ADDRESS);

  steeringHandler.setLedBrightness(ledBrightness);

  Serial.print("EPROM read brightness: ");
  Serial.println(ledBrightness);
}

void MainApp::writeLedBrightnessValueToEprom() {
  ledBrightness = steeringHandler.getLedBrightness();
  
  EEPROM.write(LED_BRIGHTNESS_VALUE_ADDRESS, ledBrightness);
  
  Serial.print("EPROM write brightness: ");
  Serial.println(ledBrightness);
}

void MainApp::blinkApplicationReady(byte useBrightness) {
  Serial.println("Application ready");
  ledBlinker.startBlinking(5, useBrightness, 50, 0, 50, 0, 500);
}

void MainApp::blinkStartCalibrating() {
  Serial.println("Start calibrating");
  ledBlinker.startBlinking(3, SIGNAL_BRIGHTNESS, 100, 0, 250, 0, 500);
}

void MainApp::blinkStartBrightnessAdjustment() {
  Serial.println("Start adjusting brightness");
  ledBlinker.startBlinking(2, SIGNAL_BRIGHTNESS, 100, 0, 250, 0, 500);
}

void MainApp::blinkWriteOK() {
  Serial.println("programming ended");
  ledBlinker.startBlinking(2, SIGNAL_BRIGHTNESS, 100, 0, 250, 0, 500);
}

void MainApp::setupSOS() {
  // Sequence definition for SOS
  byte onValue = SIGNAL_BRIGHTNESS;
  byte offValue = BYTE_MIN;

  unsigned int dotTime = 100;
  unsigned int lineTime = 300;
  unsigned int partPauseTime = 300;
  unsigned int charPauseTime = 600;

  LedBlinker::BlinkStep sosSequence[] = {
    // S ...
    { dotTime, onValue },
    { partPauseTime, offValue },
    { dotTime, onValue },
    { partPauseTime, offValue },
    { dotTime, onValue },
    { charPauseTime, offValue },

    // O ---
    { lineTime, onValue },
    { partPauseTime, offValue },
    { lineTime, onValue },
    { partPauseTime, offValue },
    { lineTime, onValue },
    { charPauseTime, offValue },

    // S ...
    { dotTime, onValue },
    { partPauseTime, offValue },
    { dotTime, onValue },
    { partPauseTime, offValue },
    { dotTime, onValue },
    { charPauseTime, offValue },

    { 500, offValue }  // pause after animation
  };
  unsigned int sequenceLength = sizeof(sosSequence) / sizeof(LedBlinker::BlinkStep);

  // Triggering the SOS flashing sequence
  noSignalBlinker.enableInfiniteLoop();
  noSignalBlinker.startBlinkingSequence(sosSequence, sequenceLength);
}

void MainApp::setupNoSignal() {
  noSignalBlinker.enableInfiniteLoop();
  noSignalBlinker.startBlinking(2, SIGNAL_BRIGHTNESS, 250, 0, 250, 0, 0);
}

String MainApp::buttonClickKindToString(RCButtonClickKind kind) {
  switch (kind) {
    case RCButtonClickKind::Click:
      return "Click";
    case RCButtonClickKind::DoubleClick:
      return "DoubleClick";
    case RCButtonClickKind::LongPress:
      return "LongPress";
    case RCButtonClickKind::ClickAndLongPress:
      return "ClickAndLongPress";
    default:
      return "Unknown";
  }
}

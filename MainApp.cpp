#include <Arduino.h>
#include <EEPROM.h>
#include "AppConstants.h"
#include "MainApp.h"
#include "LedControlBlinker.h"
#include "PWMButton.h"
#include "LedSetup.h"

MainApp::MainApp()
  : currentMode(ProgrammingModes::None), ledBrightness(0), pwmSteeringValueMin(0), pwmSteeringValueMax(0) {
}

void MainApp::init() {
  Serial.begin(9600);

  setupPins();

  pwmButton.init(PIN_PWM_BUTTON, this);
  ledBlinker.init(PIN_SIGNAL_LED);
  ledSetup.init(PIN_PWM_STEERING, PIN_SIGNAL_LED);

  readSteeringBoundsFromEprom();
  readLedBrightnessValueFromEprom();

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

  pwmButton.update();

  // I will perform the actions depending on the programming mode.
  switch (currentMode) {
    case ProgrammingModes::None:
      // I'm not in programming mode
      break;
    case ProgrammingModes::Calibrating:
      // I'm in calibration mode
      ledSetup.updateCalibration();
      break;
    case ProgrammingModes::BrightnessAdjustment:
      // I'm in LED brightness adjustment mode
      ledSetup.updateBrightnessAdjustment();
      break;
  }

  delay(LOOP_DELAY);  // Small pause to reduce CPU load
}

void MainApp::onClick(ButtonClickKind clickKind) {
  Serial.print("OnClick -> ");
  Serial.println(buttonClickKindToString(clickKind));

  // Standard single click when no programming mode is in progress.
  if (clickKind == ButtonClickKind::Click && currentMode == ProgrammingModes::None) {
    return;
  }

  // Standard double click when no programming mode is in progress.
  if (clickKind == ButtonClickKind::DoubleClick && currentMode == ProgrammingModes::None) {
    return;
  }

  // Single click in calibrating programming mode,
  // it will end the mdoe and store calibrated values.
  // It also turn mode to standart mode -> none.
  if (clickKind == ButtonClickKind::Click && currentMode == ProgrammingModes::Calibrating) {
    currentMode = ProgrammingModes::None;
    writeSteeringBoundsToEprom();
    blinkWriteOK();
    return;
  }

  // Single click in brightness adjustment programming mode,
  // it will end the mdoe and store calibrated values.
  // It also turn mode to standart mode -> none.
  if (clickKind == ButtonClickKind::Click && currentMode == ProgrammingModes::BrightnessAdjustment) {
    currentMode = ProgrammingModes::None;
    writeLedBrightnessValueToEprom();
    blinkWriteOK();
    return;
  }

  if (clickKind == ButtonClickKind::LongPress) {
    currentMode = ProgrammingModes::BrightnessAdjustment;
    blinkStartBrightnessAdjustment();
    return;
  }

  if (clickKind == ButtonClickKind::ClickAndLongPress) {
    currentMode = ProgrammingModes::Calibrating;
    blinkStartCalibrating();
    ledSetup.resetRangeLimits();
    return;
  }
}

// private methods follows

void MainApp::setupPins() {
  pinMode(PIN_PWM_BUTTON, INPUT);
  pinMode(PIN_PWM_STEERING, INPUT);
  pinMode(PIN_SIGNAL_LED, OUTPUT);
}

int MainApp::EEPROMReadInt(int address) {
  // Čtení 2 bytů z EEPROM a jejich spojení
  byte lowByte = EEPROM.read(address);
  byte highByte = EEPROM.read(address + 1);

  return (int)lowByte + ((int)highByte << 8);
}

void MainApp::EEPROMWriteInt(int address, int value) {
  // Rozdělení int na 2 byty a uložení do EEPROM
  byte lowByte = value & 0xFF;
  byte highByte = (value >> 8) & 0xFF;

  EEPROM.write(address, lowByte);
  EEPROM.write(address + 1, highByte);
}

void MainApp::readSteeringBoundsFromEprom() {
  pwmSteeringValueMin = EEPROMReadInt(STEERING_LOW_PWM_VALUE_ADDRESS);
  pwmSteeringValueMax = EEPROMReadInt(STEERING_HIGH_PWM_VALUE_ADDRESS);

  ledSetup.setRangeLimits(pwmSteeringValueMin, pwmSteeringValueMax);

  Serial.print("EPROM read bounds: ");
  Serial.print(pwmSteeringValueMin);
  Serial.print(" - ");
  Serial.println(pwmSteeringValueMax);
}

void MainApp::writeSteeringBoundsToEprom() {
  // EEPROMWriteInt(STEERING_LOW_PWM_VALUE_ADDRESS, pwmSteeringValueMin);
  // EEPROMWriteInt(STEERING_HIGH_PWM_VALUE_ADDRESS, pwmSteeringValueMax);
  Serial.print("EPROM write bounds: ");
  Serial.print(pwmSteeringValueMin);
  Serial.print(" - ");
  Serial.println(pwmSteeringValueMax);
}

void MainApp::readLedBrightnessValueFromEprom() {
  ledBrightness = EEPROM.read(LED_BRIGHTNESS_VALUE_ADDRESS);

  ledSetup.setLedBrightness(ledBrightness);

  Serial.print("EPROM read brightness: ");
  Serial.println(ledBrightness);
}

void MainApp::writeLedBrightnessValueToEprom() {
  // EEPROM.write(LED_BRIGHTNESS_VALUE_ADDRESS, ledBrightness);
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

void MainApp::blinkSOS() {
  Serial.println("SOS");
  // Sequence definition for SOS
  byte onValue = SIGNAL_BRIGHTNESS;
  byte offValue = 0;

  byte dotTime = 100;
  byte lineTime = 300;
  byte partPauseTime = 300;
  byte charPauseTime = 600;

  LedControlBlinker::BlinkStep sosSequence[] = {
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

    { 1400, offValue }  // pause after animation
  };
  unsigned int sequenceLength = sizeof(sosSequence) / sizeof(LedControlBlinker::BlinkStep);

  // spuštění sekvence blikání SOS s 5 sekundovou temnou dobou na konci
  ledBlinker.startBlinkingSequence(sosSequence, sequenceLength);
}

String MainApp::buttonClickKindToString(ButtonClickKind kind) {
  switch (kind) {
    case ButtonClickKind::Click:
      return "Click";
    case ButtonClickKind::DoubleClick:
      return "DoubleClick";
    case ButtonClickKind::LongPress:
      return "LongPress";
    case ButtonClickKind::ClickAndLongPress:
      return "ClickAndLongPress";
    default:
      return "Unknown";
  }
}

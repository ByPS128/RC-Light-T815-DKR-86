#include <Arduino.h>
#include <EEPROM.h>
#include "MainApp.h"
#include "AppConstants.h"

MainApp::MainApp()
  : rcButton(AppConstants::BUTTON_CONTROL), calibrationButton(AppConstants::BUTTON_CALIBRATION), currentMode(ProgrammingModes::None), ledBrightness(0) {
}

void MainApp::init() {
  Serial.begin(9600);

  setupPins();

  channels[0] = new RCChannel(AppConstants::PIN_PWM_STEERING);
  channels[1] = new RCChannel(AppConstants::PIN_PWM_THROTTLE);
  channels[2] = new RCChannel(AppConstants::PIN_PWM_BUTTON);

  signalValidator = new SignalValidator(channels);
  calibrationManager = new CalibrationManager(channels);
  calibrationManager->begin();

  if (!calibrationManager->isCalibrated()) {
    Serial.println("RC system not calibrated. Press calibration button to start calibration.");
  }

  calibrationButton.init(AppConstants::PIN_CALIBRATION_BUTTON);
  calibrationButton.registerSubscriber(this);

  rcButton.init(channels[2]);
  rcButton.registerSubscriber(this);

  throttleHandler.init(channels[1], AppConstants::PIN_DIGI_MOTOR_BACKWARD);
  ledBlinker.init(AppConstants::PIN_SIGNAL_LED);
  ledBlinker.registerSubscriber(this);

  steeringHandler.init(channels[0], AppConstants::PIN_PWM_LIGHT_FRONT_LED, AppConstants::PIN_SIGNAL_LED);
  readLedBrightnessValueFromEprom();

  noSignalBlinker.init(AppConstants::PIN_DIGI_LIGHT_MODE_1_LED, AppConstants::PIN_DIGI_LIGHT_MODE_2_LED, AppConstants::PIN_DIGI_LIGHT_MODE_3_LED,
                       AppConstants::PIN_DIGI_OUTER_BRAKE_LED, AppConstants::PIN_DIGI_OUTER_BRAKE_MODE,
                       AppConstants::PIN_DIGI_INNER_BRAKE_LED, AppConstants::PIN_DIGI_REVERSE_LED);
  noSignalBlinker.registerSubscriber(this);
  setupNoSignal();

  lightsController.init(LightsController::MODE_NONE,
                        ledBrightness, AppConstants::PIN_PWM_LIGHT_FRONT_LED,
                        AppConstants::PIN_DIGI_LIGHT_MODE_1_LED, AppConstants::PIN_DIGI_LIGHT_MODE_2_LED, AppConstants::PIN_DIGI_LIGHT_MODE_3_LED,
                        AppConstants::PIN_DIGI_OUTER_BRAKE_LED, AppConstants::PIN_DIGI_OUTER_BRAKE_MODE,
                        AppConstants::PIN_DIGI_INNER_BRAKE_LED, AppConstants::PIN_DIGI_REVERSE_LED);

  blinkApplicationReady(ledBrightness);
}

void MainApp::update() {
  // If the LED animation is running, I don't perform any logic.
  if (ledBlinker.updateBlinking()) {
    // LED Animation is in progress.
    // Small pause to reduce CPU load
    delay(AppConstants::LOOP_DELAY);
    return;
  }

  // Channels have to be read first of all.
  for (int i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    channels[i]->update();
  }
  calibrationManager->update();
  //describeRcChannel(2, true);

  if (signalValidator->isSignalValid()) {
    // Zpracování platného signálu
  } else {
    // Reakce na neplatný signál (např. bezpečnostní opatření)
    Serial.print("x");
    noSignalBlinker.updateBlinking();
    delay(AppConstants::LOOP_DELAY);
    return;
  }

  if (noSignalBlinker.getIsBlinking()) {
    noSignalBlinker.stop();
    lightsController.setLightsPinsByCurrentMode();
    Serial.println("Signal restored");
  }

  calibrationButton.update();

  if (calibrationManager->isInCalibrationMode()) {
    delay(AppConstants::LOOP_DELAY);
    return;
  }

  rcButton.update();
  throttleHandler.update();

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
    case ProgrammingModes::BrightnessAdjustment:
      // I'm in LED brightness adjustment mode
      steeringHandler.updateBrightnessAdjustment();
      break;
  }

  delay(AppConstants::LOOP_DELAY);  // Small pause to reduce CPU load
}

void MainApp::onButtonClick(int buttonId, ButtonClickType clickKind) {
  Serial.print("OnClick(id:");
  Serial.print(buttonId);
  Serial.print(") -> ");
  Serial.println(buttonClickTypeToString(clickKind));

  if (buttonId == AppConstants::BUTTON_CONTROL) {
    onRcPwmButtonClick(clickKind);
    return;
  }

  if (buttonId == AppConstants::BUTTON_CALIBRATION) {
    onCalibrationButtonClick(clickKind);
    return;
  }
}

void MainApp::onCalibrationButtonClick(ButtonClickType clickKind) {
  if (clickKind == ButtonClickType::LongPress && !calibrationManager->isInCalibrationMode()) {
    calibrationManager->startCalibration();
    return;
  }

  if (clickKind == ButtonClickType::DoubleClick && calibrationManager->isInCalibrationMode()) {
    calibrationManager->terminateCalibrationMode();
    return;
  }

  if (clickKind == ButtonClickType::Click && calibrationManager->isInCalibrationMode()) {
    calibrationManager->turnCalibrationMode();
    return;
  }
}

void MainApp::onRcPwmButtonClick(ButtonClickType clickKind) {
  // Standard single click when no programming mode is in progress.
  if (clickKind == ButtonClickType::Click && currentMode == ProgrammingModes::None) {
    lightsController.turnToNextLightMode();
    return;
  }

  // Standard double click when no programming mode is in progress.
  if (clickKind == ButtonClickType::DoubleClick && currentMode == ProgrammingModes::None) {
    lightsController.turnMaximumLights();
    return;
  }

  // Single click in brightness adjustment programming mode,
  // it will end the mdoe and store calibrated values.
  // It also turn mode to standart mode -> none.
  if (clickKind == ButtonClickType::Click && currentMode == ProgrammingModes::BrightnessAdjustment) {
    currentMode = ProgrammingModes::None;
    writeLedBrightnessValueToEprom();
    lightsController.setLedBirigthness(ledBrightness);
    blinkWriteOK();
    return;
  }

  if (clickKind == ButtonClickType::LongPress && currentMode == ProgrammingModes::None) {
    currentMode = ProgrammingModes::BrightnessAdjustment;
    blinkStartBrightnessAdjustment();
    return;
  }

  if (clickKind == ButtonClickType::ClickAndLongPress && currentMode == ProgrammingModes::None) {
    // not used
    return;
  }
}

void MainApp::onLedBlinkerAnimationStop(LedBlinker* instance) {
  (void)instance;
  lightsController.setLightsPinsByCurrentMode();
}

// private methods follows

void MainApp::setupPins() {
  pinMode(AppConstants::PIN_PWM_BUTTON, INPUT);
  pinMode(AppConstants::PIN_PWM_STEERING, INPUT);
  pinMode(AppConstants::PIN_PWM_THROTTLE, INPUT);
  pinMode(AppConstants::PIN_CALIBRATION_BUTTON, INPUT);
  pinMode(AppConstants::PIN_DIGI_MOTOR_BACKWARD, INPUT);

  pinMode(AppConstants::PIN_PWM_LIGHT_FRONT_LED, OUTPUT);
  pinMode(AppConstants::PIN_DIGI_LIGHT_MODE_1_LED, OUTPUT);
  pinMode(AppConstants::PIN_DIGI_LIGHT_MODE_2_LED, OUTPUT);
  pinMode(AppConstants::PIN_DIGI_LIGHT_MODE_3_LED, OUTPUT);
  pinMode(AppConstants::PIN_DIGI_OUTER_BRAKE_LED, OUTPUT);
  pinMode(AppConstants::PIN_DIGI_OUTER_BRAKE_MODE, OUTPUT);
  pinMode(AppConstants::PIN_DIGI_INNER_BRAKE_LED, OUTPUT);
  pinMode(AppConstants::PIN_DIGI_REVERSE_LED, OUTPUT);
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

void MainApp::readLedBrightnessValueFromEprom() {
  ledBrightness = EEPROM.read(AppConstants::LED_BRIGHTNESS_VALUE_ADDRESS);

  steeringHandler.setLedBrightness(ledBrightness);

  Serial.print("EPROM read brightness: ");
  Serial.println(ledBrightness);
}

void MainApp::writeLedBrightnessValueToEprom() {
  ledBrightness = steeringHandler.getLedBrightness();

  EEPROM.write(AppConstants::LED_BRIGHTNESS_VALUE_ADDRESS, ledBrightness);

  Serial.print("EPROM write brightness: ");
  Serial.println(ledBrightness);
}

void MainApp::blinkApplicationReady(byte useBrightness) {
  Serial.println("Application ready");
  ledBlinker.startBlinking(5, useBrightness, 50, 0, 50, 0, 500);
}

void MainApp::blinkStartCalibrating() {
  Serial.println("Start calibrating");
  ledBlinker.startBlinking(3, AppConstants::SIGNAL_BRIGHTNESS, 100, 0, 250, 0, 500);
}

void MainApp::blinkStartBrightnessAdjustment() {
  Serial.println("Start adjusting brightness");
  ledBlinker.startBlinking(2, AppConstants::SIGNAL_BRIGHTNESS, 100, 0, 250, 0, 500);
}

void MainApp::blinkWriteOK() {
  Serial.println("programming ended");
  ledBlinker.startBlinking(2, AppConstants::SIGNAL_BRIGHTNESS, 100, 0, 250, 0, 500);
}

void MainApp::setupSOS() {
  // Sequence definition for SOS
  byte onValue = AppConstants::SIGNAL_BRIGHTNESS;
  byte offValue = AppConstants::BYTE_MIN;

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
  noSignalBlinker.startBlinking(2, AppConstants::SIGNAL_BRIGHTNESS, 250, 0, 250, 0, 0);
}

String MainApp::buttonClickTypeToString(ButtonClickType kind) {
  switch (kind) {
    case ButtonClickType::Click:
      return "Click";
    case ButtonClickType::DoubleClick:
      return "DoubleClick";
    case ButtonClickType::LongPress:
      return "LongPress";
    case ButtonClickType::ClickAndLongPress:
      return "ClickAndLongPress";
    default:
      return "Unknown";
  }
}

void MainApp::describeRcChannel(int channelIndex, bool performLineFeed) {
  Serial.print("Channel ");
  Serial.print(channelIndex);
  Serial.print("-");
  Serial.print(channels[channelIndex]->getNamedPosition());

  Serial.print("-> min: ");
  Serial.print(channels[channelIndex]->getMin());
  Serial.print(", mid: ");
  Serial.print(channels[channelIndex]->getNeutral());
  Serial.print(", max: ");
  Serial.print(channels[channelIndex]->getMax());
  Serial.print(": ");
  Serial.print(channels[channelIndex]->getValue());

  if (performLineFeed) {
    Serial.println();
  }
}
#include <Arduino.h>
#include <EEPROM.h>
#include "MainApp.h"
#include "AppConstants.h"

MainApp::MainApp()
  : rcButton(AppConstants::BUTTON_CONTROL), calibrationButton(AppConstants::BUTTON_CALIBRATION), currentMode(ProgrammingModes::None), ledBrightness(0) {
}

void MainApp::init() {
  setupPins();

  channels[0] = new RCChannel(AppConstants::PIN_PWM_STEERING);
  channels[1] = new RCChannel(AppConstants::PIN_PWM_THROTTLE);
  channels[2] = new RCChannel(AppConstants::PIN_PWM_BUTTON);

  signalValidator = new SignalValidator(channels);
  calibrationManager = new CalibrationManager(channels);
  //calibrationManager->clearEEPROM();
  calibrationManager->init();

  calibrationButton.init(AppConstants::PIN_CALIBRATION_BUTTON);
  calibrationButton.registerSubscriber(this);

  rcButton.init(channels[2]);
  rcButton.registerSubscriber(this);

  throttleHandler.init(channels[1], AppConstants::PIN_DIGI_MOTOR_BACKWARD);
  ledBlinker.init(AppConstants::PIN_SIGNAL_LED);
  ledBlinker.registerSubscriber(this);

  steeringHandler.init(channels[0], AppConstants::PIN_PWM_LIGHT_FRONT_LED, AppConstants::PIN_SIGNAL_LED);
  readLedBrightnessValueFromEprom();

  // setup of No signal animator
  noSignalBlinker.init(AppConstants::PIN_DIGI_LIGHT_MODE_1_LED, AppConstants::PIN_DIGI_LIGHT_MODE_2_LED, AppConstants::PIN_DIGI_LIGHT_MODE_3_LED,
                       AppConstants::PIN_DIGI_OUTER_BRAKE_LED, AppConstants::PIN_DIGI_OUTER_BRAKE_MODE,
                       AppConstants::PIN_DIGI_INNER_BRAKE_LED, AppConstants::PIN_DIGI_REVERSE_LED);
  noSignalBlinker.registerSubscriber(this);
  setupNoSignal();

  // setup of Not calibrated animator
  notCalibratedBlinker.init(AppConstants::PIN_DIGI_LIGHT_MODE_1_LED, AppConstants::PIN_DIGI_LIGHT_MODE_2_LED, AppConstants::PIN_DIGI_LIGHT_MODE_3_LED,
                            AppConstants::PIN_DIGI_OUTER_BRAKE_LED, AppConstants::PIN_DIGI_OUTER_BRAKE_MODE,
                            AppConstants::PIN_DIGI_INNER_BRAKE_LED, AppConstants::PIN_DIGI_REVERSE_LED);
  notCalibratedBlinker.registerSubscriber(this);
  setupNotCalibrated();

  // Setup and set light controller to NO lights are shining.
  lightsController.init(LightsController::MODE_NONE,
                        ledBrightness, AppConstants::PIN_PWM_LIGHT_FRONT_LED,
                        AppConstants::PIN_DIGI_LIGHT_MODE_1_LED, AppConstants::PIN_DIGI_LIGHT_MODE_2_LED, AppConstants::PIN_DIGI_LIGHT_MODE_3_LED,
                        AppConstants::PIN_DIGI_OUTER_BRAKE_LED, AppConstants::PIN_DIGI_OUTER_BRAKE_MODE,
                        AppConstants::PIN_DIGI_INNER_BRAKE_LED, AppConstants::PIN_DIGI_REVERSE_LED);

  if (!calibrationManager->isCalibrated()) {
    Serial.println(F("RC system not calibrated. Press calibration button to start calibration."));
    notCalibratedBlinker.start();
    return;
  }

  updateRcChannels();
  if (!signalValidator->isSignalValid()) {
    Serial.println(F("No signal, turn on RC transmiter and receiver."));
    noSignalBlinker.start();
    return;
  }

  // When all valid, play led animation.
  blinkApplicationReady();
}

void MainApp::update() {
  // If the LED animation is running, I don't perform any logic,
  // because I need to let finish the current animation.
  if (ledBlinker.updateBlinking()) {
    // LED Animation is in progress.
    Serial.print("*");
    // Small pause to reduce CPU load
    delay(AppConstants::LOOP_DELAY);
    return;
  }

  // Channels have to be read first of all.
  updateRcChannels();
  //describeRcChannel(2, true);

  // Calibration workflow
  if (notCalibratedBlinker.getIsBlinking()) {
    notCalibratedBlinker.updateBlinking();
    if (calibrationManager->isCalibrated()) {
      notCalibratedBlinker.stop();
    }

    // give chance to calibrate
    calibrationButton.update();

    delay(AppConstants::LOOP_DELAY);
    return;
  }
  if (!calibrationManager->isCalibrated()) {
    if (!notCalibratedBlinker.getIsBlinking()) {
      notCalibratedBlinker.start();
    } else {
      notCalibratedBlinker.updateBlinking();
    }

    delay(AppConstants::LOOP_DELAY);
    return;
  }

  // Sighnal workflow
  if (!signalValidator->isSignalValid()) {
    // Reakce na neplatný signál (např. bezpečnostní opatření)
    Serial.print("x");
    noSignalBlinker.updateBlinking();

    delay(AppConstants::LOOP_DELAY);
    return;
  }

  // When signal is restored and blinker is blinking,
  // I must stop blinking and RESTORE light mode.
  if (noSignalBlinker.getIsBlinking()) {
    noSignalBlinker.stop();
    lightsController.setLightsPinsByCurrentMode();
    Serial.println(F("Signal restored"));
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

void MainApp::updateRcChannels() {
  for (int i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    channels[i]->update();
  }
  calibrationManager->update();
}

void MainApp::onButtonClick(int buttonId, ButtonClickType clickKind) {
  Serial.print(F("OnClick(id:"));
  Serial.print(buttonId);
  Serial.print(F(") -> "));
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

void MainApp::readLedBrightnessValueFromEprom() {
  ledBrightness = EEPROM.read(AppConstants::LED_BRIGHTNESS_VALUE_ADDRESS);

  steeringHandler.setLedBrightness(ledBrightness);

  Serial.print(F("EPROM read brightness: "));
  Serial.println(ledBrightness);
}

void MainApp::writeLedBrightnessValueToEprom() {
  //ledBrightness = steeringHandler.getLedBrightness();

  EEPROM.write(AppConstants::LED_BRIGHTNESS_VALUE_ADDRESS, ledBrightness);

  Serial.print(F("EPROM write brightness: "));
  Serial.println(ledBrightness);
}

void MainApp::blinkApplicationReady() {
  Serial.println(F("Application ready"));
  ledBlinker.setupBlinkingSequence(5, HIGH, 50, LOW, 50, LOW, 500);
  ledBlinker.start();
}

void MainApp::blinkStartCalibrating() {
  Serial.println(F("Start calibrating"));
  ledBlinker.setupBlinkingSequence(3, HIGH, 100, LOW, 250, LOW, 500);
  ledBlinker.start();
}

void MainApp::blinkStartBrightnessAdjustment() {
  Serial.println(F("Start adjusting brightness"));
  ledBlinker.setupBlinkingSequence(2, HIGH, 100, LOW, 250, LOW, 500);
  ledBlinker.start();
}

void MainApp::blinkWriteOK() {
  Serial.println(F("programming ended"));
  ledBlinker.setupBlinkingSequence(2, HIGH, 100, LOW, 250, LOW, 500);
  ledBlinker.start();
}

void MainApp::setupNotCalibrated() {
  // Sequence definition for SOS
  LedBlinker::BlinkStep sequence[] = {
    { 100, HIGH },
    { 50, LOW },
    { 100, HIGH },
    { 300, LOW },
  };
  unsigned int sequenceLength = sizeof(sequence) / sizeof(LedBlinker::BlinkStep);

  notCalibratedBlinker.enableInfiniteLoop();
  notCalibratedBlinker.setupBlinkingSequence(sequence, sequenceLength);
}

void MainApp::setupNoSignal() {
  noSignalBlinker.enableInfiniteLoop();
  noSignalBlinker.setupBlinkingSequence(2, HIGH, 250, LOW, 250, LOW, 0);
}

const __FlashStringHelper* MainApp::buttonClickTypeToString(ButtonClickType kind) {
  static const char click[] PROGMEM = "Click";
  static const char doubleClick[] PROGMEM = "DoubleClick";
  static const char longPress[] PROGMEM = "LongPress";
  static const char clickAndLongPress[] PROGMEM = "ClickAndLongPress";
  static const char unknown[] PROGMEM = "Unknown";
  
  switch (kind) {
    case ButtonClickType::Click:
      return reinterpret_cast<const __FlashStringHelper*>(click);
    case ButtonClickType::DoubleClick:
      return reinterpret_cast<const __FlashStringHelper*>(doubleClick);
    case ButtonClickType::LongPress:
      return reinterpret_cast<const __FlashStringHelper*>(longPress);
    case ButtonClickType::ClickAndLongPress:
      return reinterpret_cast<const __FlashStringHelper*>(clickAndLongPress);
    default:
      return reinterpret_cast<const __FlashStringHelper*>(unknown);
  }
}

void MainApp::describeRcChannel(int channelIndex, bool performLineFeed) {
  // Serial.print("Channel ");
  // Serial.print(channelIndex);
  // Serial.print("-");
  // Serial.print(channels[channelIndex]->getNamedPosition());

  // Serial.print("-> min: ");
  // Serial.print(channels[channelIndex]->getMin());
  // Serial.print(", mid: ");
  // Serial.print(channels[channelIndex]->getNeutral());
  // Serial.print(", max: ");
  // Serial.print(channels[channelIndex]->getMax());
  // Serial.print(": ");
  // Serial.print(channels[channelIndex]->getValue());

  // if (performLineFeed) {
  //   Serial.println();
  // }
}
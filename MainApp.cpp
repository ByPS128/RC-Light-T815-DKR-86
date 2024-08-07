#include <Arduino.h>
#include <EEPROM.h>
#include "MainApp.h"
#include "AppConstants.h"

MainApp::MainApp()
  : buttonHandler(1), calibrationButton(2), currentMode(ProgrammingModes::None), ledBrightness(0), pwmSteeringValueMin(0), pwmSteeringValueMax(0) {
}

void MainApp::init() {
  Serial.begin(9600);

  setupPins();

  channels[0] = new RCChannel(PIN_PWM_STEERING);
  channels[1] = new RCChannel(PIN_PWM_THROTTLE);
  channels[2] = new RCChannel(PIN_PWM_BUTTON);

  signalValidator = new SignalValidator(channels);
  calibrationManager = new CalibrationManager(channels, PIN_CALIBRATION_BUTTON, PIN_SIGNAL_LED);
  calibrationManager->begin();

  if (!calibrationManager->isCalibrated()) {
      Serial.println("RC system not calibrated. Press calibration button to start calibration.");
  }

  calibrationButton.init(PIN_CALIBRATION_BUTTON);
  calibrationButton.registerSubscriber(this);

  return;



  buttonHandler.init(PIN_PWM_BUTTON);
  buttonHandler.registerSubscriber(this);

  calibrationButton.init(PIN_CALIBRATION_BUTTON);
  calibrationButton.registerSubscriber(this);

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

  calibrationManager->update();
  calibrationButton.update();

  for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
      channels[i]->update();
  }

 if (signalValidator->isSignalValid()) {
    // Zpracování platného signálu
    for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
      Serial.print("Channel ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(channels[i]->getNamedPosition());
      Serial.print(": ");
      Serial.print(channels[i]->getValue());
      Serial.print("  |  ");
    }
    Serial.println();
  } else {
      // Reakce na neplatný signál (např. bezpečnostní opatření)
      Serial.print("x");
  }

  delay(20);

  return;



  // If the LED animation is running, I don't perform any logic.
  if (ledBlinker.updateBlinking()) {
    // LED Animation is in progress.
    // Small pause to reduce CPU load
    delay(LOOP_DELAY);
    return;
  }

  calibrationButton.update();

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

void MainApp::onButtonClick(int buttonId, ButtonClickType clickKind) {
  Serial.print("OnClick(id:");
  Serial.print(buttonId);
  Serial.print(") -> ");
  Serial.println(buttonClickTypeToString(clickKind));

  if (buttonId == 1) {
    onRcPwmButtonClick(clickKind);
    return;
  }

  if (buttonId == 2) {
    onCalibrationButtonClick(clickKind);
    return;
  }
}

void MainApp::onCalibrationButtonClick(ButtonClickType clickKind) {
  // Standard double click when no programming mode is in progress.
  //if (clickKind == ButtonClickType::DoubleClick && currentMode == ProgrammingModes::None) {
    lightsController.turnToNextLightMode();
    //return;
  //}
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

  // Single click in calibrating programming mode,
  // it will end the mdoe and store calibrated values.
  // It also turn mode to standart mode -> none.
  if (clickKind == ButtonClickType::Click && currentMode == ProgrammingModes::Calibrating) {
    currentMode = ProgrammingModes::None;
    writeSteeringBoundsToEprom();
    blinkWriteOK();
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

  if (clickKind == ButtonClickType::LongPress) {
    currentMode = ProgrammingModes::BrightnessAdjustment;
    blinkStartBrightnessAdjustment();
    return;
  }

  if (clickKind == ButtonClickType::ClickAndLongPress) {
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
  pinMode(PIN_CALIBRATION_BUTTON, INPUT);
  pinMode(PIN_DIGI_MOTOR_BACKWARD, INPUT);

  pinMode(PIN_PWM_LIGHT_FRONT_LED, OUTPUT);
  pinMode(PIN_DIGI_LIGHT_MODE_1_LED, OUTPUT);
  pinMode(PIN_DIGI_LIGHT_MODE_2_LED, OUTPUT);
  pinMode(PIN_DIGI_LIGHT_MODE_3_LED, OUTPUT);
  pinMode(PIN_DIGI_OUTER_BRAKE_LED, OUTPUT);
  pinMode(PIN_DIGI_OUTER_BRAKE_MODE, OUTPUT);
  pinMode(PIN_DIGI_INNER_BRAKE_LED, OUTPUT);
  pinMode(PIN_DIGI_REVERSE_LED, OUTPUT);
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

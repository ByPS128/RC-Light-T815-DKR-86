#include "CalibrationManager.h"
#include <EEPROM.h>

CalibrationManager::CalibrationManager(RCChannel* channels[Constants::CHANNEL_COUNT], int calibrationPin, int ledPin)
    : _calibrationPin(calibrationPin), _ledPin(ledPin), _isCalibrating(false), _calibrationStep(0), calibrationButton(2) {
    for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
        _channels[i] = channels[i];
    }
}

void CalibrationManager::begin() {
    pinMode(_calibrationPin, INPUT_PULLUP);
    pinMode(_ledPin, OUTPUT);
    loadCalibration();
	calibrationButton.init(_calibrationPin);
	calibrationButton.registerSubscriber(this);
}

void CalibrationManager::update() {
  calibrationButton.update();

    if (_isCalibrating) {
		/*
        if (millis() - _calibrationStartTime > Constants::CALIBRATION_TIMEOUT_MS) {
            finishCalibration();
        }
		*/
		for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
			_channels[i]->readAndRemember();
		}
    } else if (!isCalibrated()) {
        signalUncalibratedState();
    }
}

bool CalibrationManager::isCalibrated() const {
    for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
        if (!_channels[i]->isCalibrated()) {
            return false;
        }
    }
    return true;
}

void CalibrationManager::startCalibration() {
    _isCalibrating = true;
    _calibrationStartTime = millis();
    _calibrationStep = 0;
	for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
		_channels[i]->startCalibration();
	}
    Serial.println("Calibration started. Move all sticks to their mminimum and then to mmaximum positions several times and then press the button.");
}

void CalibrationManager::updateCalibration() {
    switch (_calibrationStep) {
        case 0:
            Serial.println("Maximum and maximum positions recorded. Release all sticks to neutral positions, wait for a second and press the button.");
            _calibrationStep++;
            break;
        case 1:
            finishCalibration();
            break;
    }
}

void CalibrationManager::finishCalibration() {
    _isCalibrating = false;
    for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
        int min = _channels[i]->getMin();
        int max = _channels[i]->getMax();
        int neutral = _channels[i]->getNeutral();
        _channels[i]->calibrate(min, max, neutral);
    }
    saveCalibration();
    Serial.println("Calibration complete and saved.");
}

void CalibrationManager::saveCalibration() {
    int address = Constants::EEPROM_DATA_START_ADDRESS;
    for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
        EEPROM.put(address, _channels[i]->getMin());
        address += sizeof(int);
        EEPROM.put(address, _channels[i]->getMax());
        address += sizeof(int);
        EEPROM.put(address, _channels[i]->getNeutral());
        address += sizeof(int);
    }
    EEPROM.put(Constants::EEPROM_MAGIC_ADDRESS, Constants::EEPROM_MAGIC_NUMBER);
}

void CalibrationManager::loadCalibration() {
    int magicNumber;
    EEPROM.get(Constants::EEPROM_MAGIC_ADDRESS, magicNumber);
    if (magicNumber != Constants::EEPROM_MAGIC_NUMBER) {
        Serial.println("No valid calibration data found in EEPROM.");
        return;
    }

    int address = Constants::EEPROM_DATA_START_ADDRESS;
    for (int i = 0; i < Constants::CHANNEL_COUNT; i++) {
        int min, max, neutral;
        EEPROM.get(address, min);
        address += sizeof(int);
        EEPROM.get(address, max);
        address += sizeof(int);
        EEPROM.get(address, neutral);
        address += sizeof(int);
        _channels[i]->calibrate(min, max, neutral);
    }
    Serial.println("Calibration data loaded from EEPROM.");
}

void CalibrationManager::signalUncalibratedState() {
    static unsigned long lastBlinkTime = 0;
    if (millis() - lastBlinkTime > 500) {
        digitalWrite(_ledPin, !digitalRead(_ledPin));
        lastBlinkTime = millis();
    }
}

void CalibrationManager::onButtonClick(int buttonId, ButtonClickType clickKind) {
  Serial.print("OnClick(id:");
  Serial.print(buttonId);
  Serial.print(") ");

  if (buttonId == 2) {
    onCalibrationButtonClick(clickKind);
    return;
  }
}

void CalibrationManager::onCalibrationButtonClick(ButtonClickType clickKind) {
	if (!_isCalibrating) {
		startCalibration();
	} else {
		updateCalibration();
	}
}


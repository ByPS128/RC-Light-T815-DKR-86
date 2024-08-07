#include "CalibrationManager.h"
#include <EEPROM.h>

CalibrationManager::CalibrationManager(RCChannel* channels[AppConstants::CHANNEL_COUNT])
  : _isCalibrating(false), _calibrationStep(0) {
  for (int i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    _channels[i] = channels[i];
  }
}

void CalibrationManager::begin() {
  loadCalibration();
}

void CalibrationManager::update() {
  if (_isCalibrating) {
    for (int i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
      _channels[i]->readAndRemember();
    }
  }
}

bool CalibrationManager::isCalibrated() const {
  for (int i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
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
  for (int i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    _channels[i]->startCalibration();
  }
  Serial.println("Calibration started. Move all sticks to their mminimum and then to mmaximum positions several times and then press the button.");
}

void CalibrationManager::updateCalibration() {
  switch (_calibrationStep) {
    case 0:
      Serial.println("Release all sticks to neutral positions, wait for a second and press the button.");
      _calibrationStep++;
      break;
    case 1:
      finishCalibration();
      break;
  }
}

void CalibrationManager::finishCalibration() {
  _isCalibrating = false;
  for (int i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    int min = _channels[i]->getMin();
    int max = _channels[i]->getMax();
    int neutral = _channels[i]->getNeutral();
    _channels[i]->calibrate(min, max, neutral);
  }
  saveCalibration();
  Serial.println("Calibration complete and saved.");
}

void CalibrationManager::saveCalibration() {
  int address = AppConstants::EEPROM_DATA_START_ADDRESS;
  for (int i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    EEPROM.put(address, _channels[i]->getMin());
    address += sizeof(int);
    EEPROM.put(address, _channels[i]->getMax());
    address += sizeof(int);
    EEPROM.put(address, _channels[i]->getNeutral());
    address += sizeof(int);
  }
  EEPROM.put(AppConstants::EEPROM_MAGIC_ADDRESS, AppConstants::EEPROM_MAGIC_NUMBER);
  Serial.println("Calibration data saved to EEPROM.");
}

void CalibrationManager::loadCalibration() {
  int magicNumber;
  EEPROM.get(AppConstants::EEPROM_MAGIC_ADDRESS, magicNumber);
  if (magicNumber != AppConstants::EEPROM_MAGIC_NUMBER) {
    Serial.println("No valid calibration data found in EEPROM.");
    return;
  }

  int address = AppConstants::EEPROM_DATA_START_ADDRESS;
  for (int i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
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

// Přidáme metodu pro vymazání EEPROM
void CalibrationManager::clearEEPROM() {
  for (int i = 0; i < AppConstants::TOTAL_EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  Serial.println("EEPROM cleared.");
}

void CalibrationManager::turnCalibrationMode() {
  if (!_isCalibrating) {
    startCalibration();
  } else {
    updateCalibration();
  }
}

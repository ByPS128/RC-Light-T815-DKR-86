#include "CalibrationManager.h"
#include <EEPROM.h>

CalibrationManager::CalibrationManager(RCChannel* channels[AppConstants::CHANNEL_COUNT])
  : _isCalibrating(false), _calibrationStep(0) {
  for (uint8_t i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    _channels[i] = channels[i];
  }
}

void CalibrationManager::init() {
  loadCalibration();
}

void CalibrationManager::update() {
  if (_isCalibrating) {
    for (uint8_t i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
      _channels[i]->readAndRemember();
    }
  }
}

bool CalibrationManager::isCalibrated() const {
  for (uint8_t i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
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
  for (uint8_t i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    _channels[i]->startCalibration();
  }
  Serial.println(F("Calibration started. Move all sticks to their mminimum and then to mmaximum positions several times and then press the button."));
}

void CalibrationManager::updateCalibration() {
  switch (_calibrationStep) {
    case 0:
      Serial.println(F("Release all sticks to neutral positions, wait for a second and press the button."));
      _calibrationStep++;
      break;
    case 1:
      finishCalibration();
      break;
  }
}

void CalibrationManager::finishCalibration() {
  _isCalibrating = false;
  for (uint8_t i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    int min = _channels[i]->getMin();
    int max = _channels[i]->getMax();
    int neutral = _channels[i]->getNeutral();
    _channels[i]->calibrate(min, max, neutral);
  }
  saveCalibration();
  Serial.println(F("Calibration complete and saved."));
}

uint16_t CalibrationManager::EEPROMReadInt(uint16_t address) {
  // Reading 2 bytes from EEPROM and their connection
  uint8_t lowByte = EEPROM.read(address);
  uint8_t highByte = EEPROM.read(address + 1);

  return (uint16_t)lowByte + ((uint16_t)highByte << 8);
}

void CalibrationManager::EEPROMWriteInt(uint16_t address, uint16_t value) {
  // Splitting int into 2 bytes and storing in EEPROM
  uint8_t lowByte = value & 0xFF;
  uint8_t highByte = (value >> 8) & 0xFF;

  EEPROM.write(address, lowByte);
  EEPROM.write(address + 1, highByte);
}

void CalibrationManager::loadCalibration() {
  uint16_t magicNumber = EEPROMReadInt(AppConstants::EEPROM_MAGIC_ADDRESS);
  EEPROM.get(AppConstants::EEPROM_MAGIC_ADDRESS, magicNumber);
  if (magicNumber != AppConstants::EEPROM_MAGIC_NUMBER) {
    Serial.println(F("No valid calibration data found in EEPROM."));

    for (uint8_t i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
      _channels[i]->calibrate(0, 255, 128);
    }

    return;
  }

  int address = AppConstants::EEPROM_DATA_START_ADDRESS;
  for (uint8_t i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    uint16_t min, max, neutral;
    EEPROM.get(address, min);
    address += sizeof(uint16_t);
    EEPROM.get(address, max);
    address += sizeof(uint16_t);
    EEPROM.get(address, neutral);
    address += sizeof(uint16_t);
    _channels[i]->calibrate(min, max, neutral);
  }
  Serial.println(F("Calibration data loaded from EEPROM."));
}

void CalibrationManager::saveCalibration() {
  int address = AppConstants::EEPROM_DATA_START_ADDRESS;
  for (uint8_t i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    EEPROM.put(address, _channels[i]->getMin());
    address += sizeof(uint16_t);
    EEPROM.put(address, _channels[i]->getMax());
    address += sizeof(uint16_t);
    EEPROM.put(address, _channels[i]->getNeutral());
    address += sizeof(uint16_t);
  }
  EEPROM.put(AppConstants::EEPROM_MAGIC_ADDRESS, AppConstants::EEPROM_MAGIC_NUMBER);
  Serial.println(F("Calibration data saved to EEPROM."));
}

// Přidáme metodu pro vymazání EEPROM
void CalibrationManager::clearEEPROM() {
  for (uint8_t i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  Serial.println(F("EEPROM cleared."));
}

void CalibrationManager::turnCalibrationMode() {
  if (!_isCalibrating) {
    startCalibration();
  } else {
    updateCalibration();
  }
}

bool CalibrationManager::isInCalibrationMode() {
  return _isCalibrating;
}

void CalibrationManager::terminateCalibrationMode() {
  for (uint8_t i = 0; i < AppConstants::CHANNEL_COUNT; i++) {
    _channels[i]->restoreCalibration();
  }
  _isCalibrating = false;
  Serial.println(F("Calibration terminated."));
}
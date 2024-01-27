#include "PwmLedBrightnessControl.h"
#include "LedControlBlinker.h"
#include <Arduino.h>
#include <EEPROM.h>

void LedSetup::blink5(byte ledBrightness) {
  stateBlinker.startBlinking(5, ledBrightness, 50, 0, 50, 0, 500);
}

void LedSetup::blink3() {
  stateBlinker.startBlinking(3, 255, 100, 0, 250, 0, 500);
}

void LedSetup::blink2() {
  stateBlinker.startBlinking(2, 255, 100, 0, 250, 0, 500);
}

void LedSetup::blinkSOS() {
  // Definice sekvence pro SOS
  LedControlBlinker::BlinkStep sosSequence[] = {
    {100, 255}, // S: tečka
    {300, 0}, // pauza
    {100, 255}, // S: tečka
    {300, 0}, // pauza
    {100, 255}, // S: tečka
    {600, false}, // pauza mezi znaky

    {300, 255}, // O: čárka
    {300, 0}, // pauza
    {300, 255}, // O: čárka
    {300, 0}, // pauza
    {300, 255}, // O: čárka
    {600, 0}, // pauza mezi znaky

    {100, 255}, // S: tečka
    {300, 0}, // pauza
    {100, 255}, // S: tečka
    {300, 0}, // pauza
    {100, 255}, // S: tečka
    {600, 0}, // pauza mezi slovy

    {1400, 0} // pauza na konci animace
  };
  unsigned int sequenceLength = sizeof(sosSequence) / sizeof(LedControlBlinker::BlinkStep);

  // spuštění sekvence blikání SOS s 5 sekundovou temnou dobou na konci
  stateBlinker.startBlinkingSequence(sosSequence, sequenceLength);
}

int LedSetup::EEPROMReadInt(int address) {
    // Čtení 2 bytů z EEPROM a jejich spojení
    byte lowByte = EEPROM.read(address);
    byte highByte = EEPROM.read(address + 1);
    
    return (int)lowByte + ((int)highByte << 8);
}

void LedSetup::EEPROMWriteInt(int address, int value) {
    // Rozdělení int na 2 byty a uložení do EEPROM
    byte lowByte = value & 0xFF;
    byte highByte = (value >> 8) & 0xFF;
    
    EEPROM.write(address, lowByte);
    EEPROM.write(address + 1, highByte);
}

void LedSetup::readSteeringBoundsFromEprom() {
  pwmSteeringValueMin = EEPROMReadInt(STEERING_LOW_PWM_VALUE_ADDRESS);
  pwmSteeringValueMax = EEPROMReadInt(STEERING_HIGH_PWM_VALUE_ADDRESS);
  Serial.print("EPROM read bounds: ");
  Serial.print(pwmSteeringValueMin);
  Serial.print(" - ");
  Serial.println(pwmSteeringValueMax);
}

void LedSetup::writeSteeringBoundsToEprom() {
  // EEPROMWriteInt(STEERING_LOW_PWM_VALUE_ADDRESS, pwmSteeringValueMin);
  // EEPROMWriteInt(STEERING_HIGH_PWM_VALUE_ADDRESS, pwmSteeringValueMax);
  Serial.print("EPROM write bounds: ");
  Serial.print(pwmSteeringValueMin);
  Serial.print(" - ");
  Serial.println(pwmSteeringValueMax);
}

void LedSetup::readLedBrightnessValueFromEprom() {
  ledBrightness = EEPROM.read(LED_BRIGHTNESS_VALUE_ADDRESS);
  Serial.print("EPROM read brightness: ");
  Serial.println(ledBrightness);
}

void LedSetup::writeLedBrightnessValueToEprom() {
  // EEPROM.write(LED_BRIGHTNESS_VALUE_ADDRESS, ledBrightness);
  Serial.print("EPROM write brightness: ");
  Serial.println(ledBrightness);
}

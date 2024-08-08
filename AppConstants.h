#pragma once

#include <Arduino.h>

namespace AppConstants {
  constexpr uint8_t LOOP_DELAY = 10;

  constexpr uint8_t BYTE_MIN = 0;
  constexpr uint8_t BYTE_MAX = 255;

  // BUTTON IDS
  constexpr uint8_t BUTTON_CONTROL = 1;
  constexpr uint8_t BUTTON_CALIBRATION = 2;

  // Assigning Arduino pins to application constants.
  constexpr uint8_t PIN_PWM_STEERING = 9;
  constexpr uint8_t PIN_PWM_THROTTLE = 10;
  constexpr uint8_t PIN_PWM_BUTTON = 11;  // In most 3-channel RC kits, the 3rd channel is in the form of a button.
  constexpr uint8_t PIN_PWM_LIGHT_FRONT_LED = 3;
  constexpr uint8_t PIN_DIGI_LIGHT_MODE_1_LED = 2;
  constexpr uint8_t PIN_DIGI_LIGHT_MODE_2_LED = 4;
  constexpr uint8_t PIN_DIGI_LIGHT_MODE_3_LED = 7;
  constexpr uint8_t PIN_CALIBRATION_BUTTON = A0;
  constexpr uint8_t PIN_DIGI_MOTOR_BACKWARD = A1;
  constexpr uint8_t PIN_DIGI_OUTER_BRAKE_LED = A3;
  constexpr uint8_t PIN_DIGI_OUTER_BRAKE_MODE = A2;
  constexpr uint8_t PIN_DIGI_INNER_BRAKE_LED = 8;
  constexpr uint8_t PIN_DIGI_REVERSE_LED = 12;
  constexpr uint8_t PIN_SIGNAL_LED = PIN_DIGI_LIGHT_MODE_1_LED;

  // Addresses to EPROM for permanent storage of set values of variables.
  constexpr uint16_t LED_BRIGHTNESS_VALUE_ADDRESS = 0x00F0;

  // Constants for Calibration Manager
  const uint16_t EEPROM_MAGIC_ADDRESS = 0x0000;                                  // Začínáme od adresy 128
  const uint16_t EEPROM_MAGIC_NUMBER = 0xABCD;                                   // Unikátní číslo pro identifikaci platných dat
  const uint16_t EEPROM_DATA_START_ADDRESS = EEPROM_MAGIC_ADDRESS + sizeof(int); // Začátek dat hned za magic number
  const uint8_t CHANNEL_COUNT = 3;                                              // Předpokládaný počet kanálů (upravte podle potřeby)

  // Výpočet velikosti dat pro jeden kanál
  const uint16_t CHANNEL_DATA_SIZE = CHANNEL_COUNT * sizeof(int);  // min, max, neutral jsou int

  // Celková velikost dat v EEPROM
  const uint16_t TOTAL_EEPROM_SIZE = EEPROM_DATA_START_ADDRESS + (CHANNEL_COUNT * CHANNEL_DATA_SIZE);

  // Constants for RC signal validation
  const uint16_t MIN_PULSE_WIDTH_US = 900;
  const uint16_t MAX_PULSE_WIDTH_US = 2100;
}

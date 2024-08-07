#pragma once

#include <Arduino.h>

namespace AppConstants {
  constexpr byte BYTE_MIN = 0;
  constexpr byte BYTE_MAX = 255;

  // BUTTON IDS
  constexpr int BUTTON_CONTROL = 1;
  constexpr int BUTTON_CALIBRATION = 2;

  // Assigning Arduino pins to application constants.
  constexpr byte PIN_PWM_STEERING = 9;
  constexpr byte PIN_PWM_THROTTLE = 10;
  constexpr byte PIN_PWM_BUTTON = 11;  // In most 3-channel RC kits, the 3rd channel is in the form of a button.
  constexpr byte PIN_PWM_LIGHT_FRONT_LED = 3;
  constexpr byte PIN_DIGI_LIGHT_MODE_1_LED = 2;
  constexpr byte PIN_DIGI_LIGHT_MODE_2_LED = 4;
  constexpr byte PIN_DIGI_LIGHT_MODE_3_LED = 7;
  constexpr byte PIN_CALIBRATION_BUTTON = A0;
  constexpr byte PIN_DIGI_MOTOR_BACKWARD = A1;
  constexpr byte PIN_DIGI_OUTER_BRAKE_LED = A3;
  constexpr byte PIN_DIGI_OUTER_BRAKE_MODE = A2;
  constexpr byte PIN_DIGI_INNER_BRAKE_LED = 8;
  constexpr byte PIN_DIGI_REVERSE_LED = 12;
  constexpr byte PIN_SIGNAL_LED = PIN_DIGI_LIGHT_MODE_1_LED;

  constexpr byte SIGNAL_BRIGHTNESS = BYTE_MAX;
  constexpr int LOOP_DELAY = 10;

  // Addresses to EPROM for permanent storage of set values of variables.
  constexpr int LED_BRIGHTNESS_VALUE_ADDRESS = 0x0000;
  constexpr int STEERING_LOW_PWM_VALUE_ADDRESS = 0x0001;
  constexpr int STEERING_HIGH_PWM_VALUE_ADDRESS = 0x0003;

  // Constants for Calibration Manager
  const int EEPROM_MAGIC_ADDRESS = 0xF0;                          // Začínáme od adresy 128
  const int EEPROM_MAGIC_NUMBER = EEPROM_MAGIC_ADDRESS + 0xABCD;  // Unikátní číslo pro identifikaci platných dat
  const int EEPROM_DATA_START_ADDRESS = sizeof(int);              // Začátek dat hned za magic number
  const int CHANNEL_COUNT = 3;                                    // Předpokládaný počet kanálů (upravte podle potřeby)

  // Výpočet velikosti dat pro jeden kanál
  const int CHANNEL_DATA_SIZE = 3 * sizeof(int);  // min, max, neutral jsou int

  // Celková velikost dat v EEPROM
  const int TOTAL_EEPROM_SIZE = EEPROM_DATA_START_ADDRESS + (CHANNEL_COUNT * CHANNEL_DATA_SIZE);

  // Constants for RC signal validation
  const int MIN_PULSE_WIDTH_US = 900;
  const int MAX_PULSE_WIDTH_US = 2100;
}

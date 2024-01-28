#pragma once

#include <Arduino.h>

constexpr byte BYTE_ZERO = 0;
constexpr byte BYTE_MIN = 0;
constexpr byte BYTE_MAX = 255;
constexpr byte BYTE_MID = 128;

// Assigning Arduino pins to application constants.
constexpr byte PIN_SIGNAL_LED = 10;
constexpr byte PIN_PWM_STEERING = 5;
constexpr byte PIN_PWM_BUTTON = 3;  // In most 3-channel RC kits, the 3rd channel is in the form of a button.

constexpr byte PIN_DIGI_LIGHT_MODE_1_LED = 10;
constexpr byte PIN_DIGI_LIGHT_MODE_2_LED = 11;
constexpr byte PIN_DIGI_LIGHT_MODE_3_LED = 12;
constexpr byte PIN_PWM_LIGHT_BREAK_LED = 11;
constexpr byte PIN_DIGI_LIGHT_BREAK_LED = 12;
constexpr byte PIN_DIGI_LIGHT_REVERSE_LED = 12;

constexpr byte SIGNAL_BRIGHTNESS = BYTE_MAX;
constexpr int LOOP_DELAY = 10;

// Addresses to EPROM for permanent storage of set values of variables.
constexpr int LED_BRIGHTNESS_VALUE_ADDRESS = 0x0000;
constexpr int STEERING_LOW_PWM_VALUE_ADDRESS = 0x0001;
constexpr int STEERING_HIGH_PWM_VALUE_ADDRESS = 0x0003;

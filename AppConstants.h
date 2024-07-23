#pragma once

#include <Arduino.h>

constexpr byte BYTE_ZERO = 0;
constexpr byte BYTE_MIN = 0;
constexpr byte BYTE_MAX = 255;
constexpr byte BYTE_MID = 128;

constexpr unsigned long SIGNAL_VALID_LOW_VALUE_THRESHOLD = 900;
constexpr unsigned long SIGNAL_VALID_HIGH_VALUE_THRESHOLD = 2100;

// Assigning Arduino pins to application constants.
constexpr byte PIN_PWM_STEERING = 9;
constexpr byte PIN_PWM_THROTTLE = 10;
constexpr byte PIN_PWM_BUTTON = 11;  // In most 3-channel RC kits, the 3rd channel is in the form of a button.
constexpr byte PIN_PWM_LIGHT_FRONT_LED = 3;
constexpr byte PIN_DIGI_LIGHT_MODE_1_LED = 2;
constexpr byte PIN_DIGI_LIGHT_MODE_2_LED = 4;
constexpr byte PIN_DIGI_LIGHT_MODE_3_LED = 7;
constexpr byte PIN_DIGI_MOTOR_BACKWARD = A1;
constexpr byte PIN_DIGI_OUTER_BRAKE_LED = A3;
constexpr byte PIN_DIGI_OUTER_BRAKE_MODE = A2;
constexpr byte PIN_DIGI_INNER_BRAKE_LED = 8;
constexpr byte PIN_DIGI_REVERSE_LED = 12;//A4;//12;
constexpr byte PIN_SIGNAL_LED = PIN_DIGI_LIGHT_MODE_1_LED;

constexpr byte SIGNAL_BRIGHTNESS = BYTE_MAX;
constexpr int LOOP_DELAY = 10;

// Addresses to EPROM for permanent storage of set values of variables.
constexpr int LED_BRIGHTNESS_VALUE_ADDRESS = 0x0000;
constexpr int STEERING_LOW_PWM_VALUE_ADDRESS = 0x0001;
constexpr int STEERING_HIGH_PWM_VALUE_ADDRESS = 0x0003;

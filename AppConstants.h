#pragma once

#include <Arduino.h>

// Assigning Arduino pins to application constants.
constexpr byte PIN_SIGNAL_LED = 10;
constexpr byte PIN_PWM_STEERING = 5;
constexpr byte PIN_PWM_BUTTON = 3; // In most 3-channel RC kits, the 3rd channel is in the form of a button.

constexpr byte SIGNAL_BRIGHTNESS = 255;
constexpr int dblClickInterval = 200;
constexpr int LOOP_DELAY = 10;

// Addresses to EPROM for permanent storage of set values of variables.
constexpr int LED_BRIGHTNESS_VALUE_ADDRESS = 0x0000;
constexpr int STEERING_LOW_PWM_VALUE_ADDRESS = 0x0001;
constexpr int STEERING_HIGH_PWM_VALUE_ADDRESS = 0x0003;
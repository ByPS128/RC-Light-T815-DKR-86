#pragma once

namespace Constants {
    // Constants for Calibration Manager
    const int EEPROM_MAGIC_NUMBER = 99;
    const int EEPROM_MAGIC_ADDRESS = 128;
    const int EEPROM_DATA_START_ADDRESS = 129;
    const int CHANNEL_COUNT = 3;
    const int CALIBRATION_TIMEOUT_MS = 30000; // 30 seconds
    const int BUTTON_DEBOUNCE_MS = 50;
    const int NEUTRAL_TOLERANCE = 50; // Tolerance for neutral position

    // Constants for RC signal validation
    const unsigned long MAX_PULSE_GAP_US = 100000; // 100 ms
    const int MIN_PULSE_WIDTH_US = 900;
    const int MAX_PULSE_WIDTH_US = 2100;
    const int FAILSAFE_PULSE_WIDTH_US = 800; // Příklad hodnoty, může se lišit podle nastavení
}
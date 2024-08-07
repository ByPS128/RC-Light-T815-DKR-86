#pragma once

namespace Constants {
    // Constants for Calibration Manager
    const int EEPROM_MAGIC_ADDRESS = 0xF0;  // Začínáme od adresy 128
    const int EEPROM_MAGIC_NUMBER = EEPROM_MAGIC_ADDRESS + 0xABCD;  // Unikátní číslo pro identifikaci platných dat
    const int EEPROM_DATA_START_ADDRESS = sizeof(int);  // Začátek dat hned za magic number
    const int CHANNEL_COUNT = 3;  // Předpokládaný počet kanálů (upravte podle potřeby)

    // Výpočet velikosti dat pro jeden kanál
    const int CHANNEL_DATA_SIZE = 3 * sizeof(int);  // min, max, neutral jsou int

    // Celková velikost dat v EEPROM
    const int TOTAL_EEPROM_SIZE = EEPROM_DATA_START_ADDRESS + (CHANNEL_COUNT * CHANNEL_DATA_SIZE);
    
    const int CALIBRATION_TIMEOUT_MS = 30000; // 30 seconds
    const int BUTTON_DEBOUNCE_MS = 50;
    const int NEUTRAL_TOLERANCE = 50; // Tolerance for neutral position

    // Constants for RC signal validation
    const unsigned long MAX_PULSE_GAP_US = 100000; // 100 ms
    const unsigned long MIN_PULSE_WIDTH_US = 900;
    const unsigned long MAX_PULSE_WIDTH_US = 2100;
    const int FAILSAFE_PULSE_WIDTH_US = 800; // Příklad hodnoty, může se lišit podle nastavení
}